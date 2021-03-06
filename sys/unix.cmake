include(CheckCXXSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFiles)

#######################################################################################################################################

check_cxx_source_runs("#include <sys/types.h>\n int main(){gettid();return 0;}" LEMON_HAS_GETTID)

#######################################################################################################################################
# check the random function implement
CHECK_CXX_SOURCE_RUNS("#include <fstream>\n int main(){std::ifstream f(\"/dev/urandom\");return f.is_open()?0:1;}" LEMON_RANDOM_URANDOM)

CHECK_CXX_SOURCE_RUNS("#include <fstream>\n int main(){std::ifstream f(\"/opt/openssl/prngd/prngd\");return f.is_open()?0:1;}" LEMON_RANDOM_PRNGD)

if(NOT LEMON_RANDOM_URANDOM AND NOT LEMON_RANDOM_PRNGD)
  lemon_error("not found urandom,random functions not support on this platform")
endif()

#######################################################################################################################################
# check if the gcc compiler support atomic_t function
CHECK_CXX_SOURCE_RUNS("int main(){int i = 0;__sync_add_and_fetch(&i,1);return 0;}" LEMON_USE_GCC_BUIDIN_ATOMIC_FUNCTION)

check_cxx_source_runs("#include<ext/atomicity.h>\n int main(){int i = 0;__gnu_cxx::__exchange_and_add(&i,1);return 0;}" LEMON_USE_GCC_EXT_ATOMICITY_H)

check_cxx_source_runs("#include<bits/atomicity.h>\n int main(){int i = 0;__gnu_cxx::__exchange_and_add(&i,1);return 0;}" LEMON_USE_GCC_BITS_ATOMICITY_H)

if(NOT LEMON_USE_GCC_BUIDIN_ATOMIC_FUNCTION AND NOT LEMON_USE_GCC_EXT_ATOMICITY_H AND NOT LEMON_USE_GCC_BITS_ATOMICITY_H)
  lemon_error("atomic api not support on this paltform,please use GCC compile lemon libraries")
endif()

#######################################################################################################################################
# check the ld library head file
CHECK_INCLUDE_FILES(dlfcn.h LEMON_HAS_DLFCN_H)


#######################################################################################################################################
# check the coroutine library implement technology

if(APPLE)
  set(LEMON_COROUTINE_DEFAULT ASM)
else()
  set(LEMON_COROUTINE_DEFAULT UCONTEXT)
endif(APPLE)

lemon_option(
  LEMON_COROUTINE
  DEFAULT ${LEMON_COROUTINE_DEFAULT}
  VALUES UCONTEXT ASM
  DESCRIPTION "special coroutine library implement")

if(LEMON_COROUTINE_UCONTEXT)
  check_include_files(ucontext.h  LEMON_HAS_UCONTEXT_H)
  if(NOT LEMON_HAS_UCONTEXT_H)
    lemon_error("not found ucontext.h,you can try build coroutine library by asm")
  endif()
endif()

if(LEMON_COROUTINE_ASM)
  set(LEMON_USE_BUILDIN_UCONTEXT TRUE)
  lemon_project_sources(${PROJECT_SOURCE_DIR}/sources/lemon/sys/ucontext.s)
endif()


########################################################################################################################################
#                        
# check the text converter technology
#
########################################################################################################################################


lemon_option(
  LEMON_TEXT_CONVERTER
  DEFAULT ICONV
  VALUES ICONV ICU
  DESCRIPTION "the lemon library text converter engine")

if(LEMON_TEXT_CONVERTER_ICONV)
  lemon_find_package(ICONV REQUIRED)
endif()

if(LEMON_TEXT_CONVERTER_ICU)
  lemon_find_package(ICU REQUIRED)
endif()



#########################################################################################################################################
# check uuid implement 

set(LEMON_UUID_BUILDIN TRUE)


lemon_project_link_libs(dl rt)

##########################################################################################################################################
# 

set(LEMON_SUPPORT_IPV6 TRUE)