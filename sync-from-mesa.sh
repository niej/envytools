#!/bin/bash

set -e
#set -x

mesa="$1"

if [ ! -d "$mesa" ]; then
	echo "using: $0 <path-to-mesa>"
	exit 1
fi

cd `dirname $0`
envytools=`pwd`

# TODO sync more of the mesa gitlab-ci.. but for now just syncing
# the reference decodes should be enough to not break when regs
# are updated
sync_paths="
	afuc
	common/disasm.h
	decode
	isa
	ir2
	ir3/disasm-a3xx.c
	ir3/instr-a3xx.h
	ir3/regmask.h
	registers
	rnn
	.gitlab-ci/reference/crash.log
	.gitlab-ci/reference/es2gears-a320.log
	.gitlab-ci/reference/fd-clouds.log
	.gitlab-ci/reference/glxgears-a420.log
	.gitlab-ci/reference/shadow.log
"

# src path in mesa tree to find $sync_paths:
mesa_sync_path="src/freedreno"

inmesa() {
	log "  - in mesa: $*"
	(cd $mesa; $*)
}

inmesaq() {
	(cd $mesa; $*)
}

inenvytools() {
	log "  - in envytools: $*"
	(cd $envytools; $*)
}

inenvytoolsq() {
	(cd $envytools; $*)
}

log() {
	echo $*
}

commitinfo() {
	commitid=$1
	format=$2
	inmesaq git show $commitid -s --format="$format"
}

syncit() {
	commitid=$1

	log "syncing from: $commitid"

	inmesa git checkout $commitid
	for p in $sync_paths; do
		log " + syncing: $p"
		inenvytools rm -rf $p
		inmesa cp -r $mesa_sync_path/$p $envytools/$p
		
		# HACK: this file doesn't exist on mesa side (yet)
		inenvytools git checkout registers/.gitignore

		inenvytools git add $p
	done

	inenvytoolsq echo $commitid > .lastsync
	inenvytools git add .lastsync

	author=`commitinfo $commitid '%an'`
	email=`commitinfo $commitid '%ae'`
	date=`commitinfo $commitid '%aD'`
	subject=`commitinfo $commitid '%s'`
	body=`commitinfo $commitid '%b'`

	git commit \
		--author="$author \<$email\>" \
		--date="$date" \
		-m "$subject" \
		-m "Sync from mesa commit $commitid" \
		-m "$body"

	# Do local build + CI to make sure things are ok:
	inenvytools ninja -C debug install
	inenvytools ./.gitlab-ci/genoutput.sh
	inenvytools diff -r -I '^Reading' -I '^Parsing' -I 'Assertion' .gitlab-ci/reference .gitlab-ci/out
}


mesapaths=""
for p in $sync_paths; do
	mesapaths="$mesapaths $mesa_sync_path/$p"
done

# Find commits to sync:
lastsync=`inenvytoolsq cat .lastsync`

inmesa git reset --hard HEAD
inmesa git checkout master
inmesa git pull --rebase origin master

log "Commits available to sync:"
inmesa git --no-pager log --oneline ${lastsync}..HEAD -- $mesapaths

commitids=`inmesaq git log --reverse --format='%H' ${lastsync}..HEAD -- $mesapaths`

for commitid in $commitids; do
	syncit $commitid
done

# For now, push to syncit branch on success.  I might switch this to push
# directly to master after it has proven reliable for some time
inenvytools git push -f gitlab syncit
