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
)

list(
	APPEND c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS
	
	-std=gnu++17
	-fno-rtti
)
