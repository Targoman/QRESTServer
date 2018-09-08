#!/bin/sh

BasePath=$(realpath $1)
DEPS_BUILT=$2

if [ -f $DEPS_BUILT ];
  exit 0
fi

pushd $BasePath >/dev/null 2>&1

if [ -f .gitmodules ]; then
  Deps=$(grep "\[submodule " .gitmodules | cut -d ' ' -f 2 | tr -d '\"\]')
  for Dep in $Deps; do
    echo "=====================> Building $Dep <========================"
    pushd $Dep
      make distclean
      qmake-qt5 PREFIX=$BasePath/out
      make install -j
    popd
  done
fi

touch $$DEPS_BUILT

popd >/dev/null 2>&1
