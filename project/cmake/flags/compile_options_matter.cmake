#remove flags for compiling matter library

list(
	REMOVE_ITEM c_GLOBAL_COMMON_COMPILE_C_OPTIONS
	# -fno-strict-aliasing
	# -fno-strict-overflow
	# -fno-delete-null-pointer-checks
	-Wstrict-prototypes
	-Wundef
	-Werror
	--save-temps
)

list(
	REMOVE_ITEM c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS
	-std=c++11
	-fno-use-cxa-atexit
	-Werror
)

# add compile flags

# Compile Warnings
list(
	APPEND c_GLOBAL_COMMON_COMPILE_C_OPTIONS
	-Wno-sign-compare
	-Wno-unused-function
	-Wno-unused-but-set-variable
	-Wno-unused-variable
	-Wno-unused-label
	-Wno-deprecated-declarations
	-Wno-unused-parameter
	-Wno-format
	-Wno-format-nonliteral
	-Wno-format-security
	-Wno-address
	-Wno-stringop-truncation
	-Wno-type-limits
)

list(
	APPEND c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS

	-std=gnu++17
	-fno-rtti
	-Wno-register
)

# RTL8720F's ameba_boot.h uses C11-only _Static_assert, but ameba.h includes it
# unconditionally and CHIP's C++ sources pull in ameba.h. Map it to C++11's
# static_assert for C++ TUs only -- the same one-token rename hw_sha_context uses.
# Scoped to C++ so the C11 keyword keeps working in C sources.
if(CONFIG_RTL8720F)
	list(
		APPEND c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS
		-D_Static_assert=static_assert
	)
endif()
