#!/usr/bin/env bash
set -euo pipefail

VERSION=$(git describe --tags --always --dirty 2>/dev/null || echo "unknown")
ARCHIVE="thulogin-${VERSION}.tar.gz"
PREFIX="thulogin-${VERSION}"

echo "Packaging thulogin (${VERSION})..."

tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

mkdir -p "${tmpdir}/${PREFIX}"
cp -r Makefile README.md LICENSE src include lib "${tmpdir}/${PREFIX}/"

tar czf "${ARCHIVE}" \
    -C "${tmpdir}" \
    --exclude='*.o' \
    --exclude='*.dbg.o' \
    --exclude='.DS_Store' \
    "${PREFIX}"

echo "Created: ${ARCHIVE}"
ls -lh "${ARCHIVE}"
