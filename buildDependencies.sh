#!/bin/sh

BasePath=$(realpath $1)
DEPS_BUILT=$2

if [ -f $DEPS_BUILT ];then
  exit 0
fi

pushd $BasePath >/dev/null 2>&1

if [ -f .gitmodules ]; then
  Deps=$(grep "\[submodule " .gitmodules | cut -d ' ' -f 2 | tr -d '\"\]')
  for Dep in $Deps; do
    echo "\n\n\n=====================> Building $Dep <========================\n\n"
    pushd $Dep
      if [ -n $(ls "*.pro" 2>/dev/null) ]; then
          make distclean
          qmake-qt5 PREFIX=$BasePath/out
          make install -j
      elif [ -n $(ls "CMakeLists.txt" 2>/dev/null) ];then
          mkdir -p buildForProject
          pushd buildForProject
            camke -DCMAKE_INSTALL_PREFIX:PATH=$BasePath/out ..
            make install -j
          popd
      else
        echo "\n\n\n\n[WARNING] Project: $Dep type could not be determined so will not be compiled"
      fi
    popd
  done
fi

touch $BasePath/$DEPS_BUILT

popd >/dev/null 2>&1
