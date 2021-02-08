#!/bin/bash

set -e

CXX_PCH="wx\\/wxprec\\.h\\.gch"
OBJCXX_PCH="wx\\/wxprec\\.h\\.objc\\.gch"

echo $OBJCXX_PCH

FILE=Makefile

PCH_PATH_PREFIX="\\.\\/\\.pch\\/wxprec_"

cp Makefile Makefile.bak


# -BASELIB_CXXFLAGS = $(__baselib_PCH_INC)[...]
# +BASELIB_CXXFLAGS = -include-pch ./.pch/wxprec_baselib/wx/wxprec.h.gch[...]

sed -i '' "s/^\([A-Z]*_CXXFLAGS = \)\$(__\([a-z]*\)_PCH_INC)/\1-include-pch ${PCH_PATH_PREFIX}\2\/${CXX_PCH}/" $FILE


# -BASELIB_OBJCXXFLAGS = $(__baselib_PCH_INC)[...]
# +BASELIB_OBJCXXFLAGS = -include-pch ./.pch/wxprec_baselib/wx/wxprec.h.objc.gch[...]

sed -i '' "s/^\([A-Z]*_OBJCXXFLAGS = \)\$(__\([a-z]*\)_PCH_INC)/\1-include-pch ${PCH_PATH_PREFIX}\2\/${OBJCXX_PCH}/" $FILE


# -	$(BK_MAKE_PCH) ./.pch/wxprec_baselib/wx/wxprec.h.gch wx/wxprec.h $(CXX) [1]
# +	$(BK_MAKE_PCH) ./.pch/wxprec_baselib/wx/wxprec.h.gch wx/wxprec.h $(CXX) -x c++-header [1]
# +
# +./.pch/wxprec_baselib/wx/wxprec.h.objc.gch:
# +	$(BK_MAKE_PCH) ./.pch/wxprec_baselib/wx/wxprec.h.objc.gch wx/wxprec.h $(CXX) -x objective-c++-header [1]

sed -i '' "s/^\(	\$(BK_MAKE_PCH) ${PCH_PATH_PREFIX}\)\([a-z]*\)\/${CXX_PCH} \(wx\/wxprec\.h \$(CXX)\) \(\$([A-Z]*_CXXFLAGS)\)/\1\2\/$CXX_PCH \3 -x c++-header \4\\
\\
.\/.pch\/wxprec_\2\/${OBJCXX_PCH}:\\
\1\2\/${OBJCXX_PCH} \3 -x objective-c++-header \4/" $FILE


# Add ObjC++ PCH dependency.

# -	= ./.pch/wxprec_baselib/wx/wxprec.h.gch
# +	= ./.pch/wxprec_baselib/wx/wxprec.h.gch ./.pch/wxprec_baselib/wx/wxprec.h.objc.gch

sed -i '' "s/^	= \(${PCH_PATH_PREFIX}\)\([a-z]*\/\)${CXX_PCH}$/	= \1\2${CXX_PCH} \1\2${OBJCXX_PCH}/" $FILE

diff -u Makefile.bak Makefile
