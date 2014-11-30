#  Copyright (C) 2009-2014 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3, or (at your option) any
# later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.

# Generates compressed tables for types for i386 builtin functions.

function do_error(string) {
    print FILENAME ":" FNR ": " string > "/dev/stderr"
    errors = 1
}

function check_type(string) {
    if (!(string in type_hash))
	do_error("undefined type code " string)
}

# We can significantly reduce the size of the read-only tables
# by forcing the compiler to use a smaller implementation type
# for the enumerations.
function attribute_mode(count) {
    # ??? Except that we get strange "comparison always false" warnings
    # for comparisons between different elements of the enumeration.
    #    print "#ifdef __GNUC__"
    #    if (count < 256)
    #	print "  __attribute__((__mode__(__QI__)))"
    #    else
    #	print "  __attribute__((__mode__(__HI__)))"
    #    print "#endif"
}

BEGIN {
    FS = "[() \t,]+"
   
    prim_defs = 0
    vect_defs = 0
    ptr_defs = 0
    cptr_defs = 0
    func_defs = 0
    func_args = 0
    alias_defs = 0
}

# Skip blank lines or comments.
/^[ \t]*(#|$)/ {
    next
}

$1 == "DEF_PRIMITIVE_TYPE" {
    if (NF == 4) {
	type_hash[$2] = 1
	prim_name[prim_defs] = $2
	prim_base[prim_defs] = $3
	prim_defs++
    } else
	do_error("DEF_PRIMITIVE_TYPE expected 2 arguments")
    next
}

$1 == "DEF_VECTOR_TYPE" {
    if (NF == 4 || NF == 5) {
	check_type($3)
	type_hash[$2] = 1
	vect_name[vect_defs] = $2
	vect_base[vect_defs] = $3
	vect_mode[vect_defs] = (NF == 5 ? $4 : $2)
	vect_defs++
    } else
	do_error("DEF_VECTOR_TYPE expected 2 arguments")
    next
}

$1 == "DEF_POINTER_TYPE" {
    if (NF == 4) {
	check_type($3)
	type_hash[$2] = 1
	ptr_name[ptr_defs] = $2
	ptr_base[ptr_defs] = $3
	ptr_defs++
    } else if (NF == 5) {
	check_type($3)
	if ($4 == "CONST") {
	    type_hash[$2] = 1
	    cptr_name[cptr_defs] = $2
	    cptr_base[cptr_defs] = $3
	    cptr_defs++
	} else
	    do_error("invalid qualifier \"" $4 "\"")
    }
    else
	do_error("DEF_POINTER_TYPE expected 2 or 3 arguments")
    next
}

$1 == "DEF_FUNCTION_TYPE" {
    func_start[func_defs] = func_args
    for (i = 2; i < NF; ++i) {
	check_type($i)
	func_types[func_args++] = $i
    }

    if (NF < 3)
	do_error("DEF_FUNCTION_TYPE expected at least 1 argument")
    else if (NF == 3)
	name = $2 "_FTYPE_VOID"
    else {
	name = $2 "_FTYPE"
	for (i = 3; i < NF; ++i)
	    name = name "_" $i
    }
    func_hash[name] = 1
    func_name[func_defs++] = name
    next
}

$1 == "DEF_FUNCTION_TYPE_ALIAS" {
    if (NF == 4) {
	if ($2 in func_hash) {
	    alias_base[alias_defs] = $2
	    alias_name[alias_defs] = $2 "_" $3
	    alias_defs++
	} else
	    do_error("undefined function code " $2)
    } else
	do_error("DEF_FUNCTION_TYPE_ALIAS expected 2 arguments")
    next
}

{
    do_error("unknown directive \"" $1 "\"");
}

END {
    if (errors)
	exit 1

    print "/* This file is auto-generated by i386-builtin-types.awk.  */\n"

    # This first enumeration contains all of the non-function types.
    print "enum ix86_builtin_type {"
    for (i = 0; i < prim_defs; ++i)
	print "  IX86_BT_" prim_name[i] ","
    print "  IX86_BT_LAST_PRIM = IX86_BT_" prim_name[i-1] ","
    for (i = 0; i < vect_defs; ++i)
	print "  IX86_BT_" vect_name[i] ","
    print "  IX86_BT_LAST_VECT = IX86_BT_" vect_name[i-1] ","
    for (i = 0; i < ptr_defs; ++i)
	print "  IX86_BT_" ptr_name[i] ","
    print "  IX86_BT_LAST_PTR = IX86_BT_" ptr_name[i-1] ","
    for (i = 0; i < cptr_defs; ++i)
	print "  IX86_BT_" cptr_name[i] ","
    print "  IX86_BT_LAST_CPTR = IX86_BT_" cptr_name[i-1] "\n}"
    attribute_mode(prim_defs + vect_defs + ptr_defs + cptr_defs)
    print ";\n\n"

    # We can't tabularize the initialization of the primitives, since
    # at least one of them is created via a local variable.  That's ok,
    # just create a nice big macro to do all the work.
    print "#define DEFINE_BUILTIN_PRIMITIVE_TYPES \\"
    for (i = 0; i < prim_defs; ++i) {
	printf "  ix86_builtin_type_tab[(int)IX86_BT_" prim_name[i] \
	    "] = " prim_base[i]
	if (i < prim_defs - 1)
	    print ", \\"
    }
    print "\n\n"

    # The vector types are defined via two tables defining the real
    # machine mode and the builtin primitive type.  We use two tables
    # rather than a structure to avoid structure padding and save space.
    print "static const machine_mode ix86_builtin_type_vect_mode[] = {"
    for (i = 0; i < vect_defs; ++i) {
	if (i == 0)
	    printf "  "
	else if (i % 6 == 0)
	    printf ",\n  "
	else
	    printf ", "
	printf vect_mode[i] "mode"
    }
    print "\n};\n\n"

    print "static const enum ix86_builtin_type " \
	"ix86_builtin_type_vect_base[] = {"
    for (i = 0; i < vect_defs; ++i) {
	if (i == 0)
	    printf "  "
	else if (i % 4 == 0)
	    printf ",\n  "
	else
	    printf ", "
	printf "IX86_BT_" vect_base[i]
    }
    print "\n};\n\n"

    # The pointer types are defined via a single table defining the
    # builtin primitive type.  The const-ness of the pointer is taken
    # from the enumeration value > IX86_BT_LAST_PTR.
    print "static const enum ix86_builtin_type " \
	"ix86_builtin_type_ptr_base[] = {"
    for (i = 0; i < ptr_defs; ++i) {
	if (i == 0)
	    printf " "
	else if (i % 4 == 0)
	    printf "\n "
	printf " IX86_BT_" ptr_base[i] ","
    }
    print "\n  /* pointer-to-constant defs start here */"
    for (i = 0; i < cptr_defs; ++i) {
	if (i == 0)
	    printf "  "
	else if (i % 4 == 0)
	    printf ",\n  "
	else
	    printf ", "
	printf "IX86_BT_" cptr_base[i]
    }
    print "\n};\n\n"

    # This second enumeration contains all of the function types.
    print "enum ix86_builtin_func_type {"
    for (i = 0; i < func_defs; ++i)
	print "  " func_name[i] ","
    print "  IX86_BT_LAST_FUNC = " func_name[i-1] ","
    for (i = 0; i < alias_defs; ++i)
	print "  " alias_name[i] ","
    print "  IX86_BT_LAST_ALIAS = " alias_name[i-1] "\n}"
    attribute_mode(func_defs + alias_defs)
    print ";\n\n"

    # The function types are defined via two tables.  The first contains
    # ranges consiting of the function's return type, followed by all of
    # the function argument types.  The ranges for all of the builtin
    # functions are smooshed together in the same array.  The second array
    # contains, for each builtin, the index of the function's return type
    # within the first array.
    print "static const enum ix86_builtin_type ix86_builtin_func_args[] = {"
    for (i = 0; i < func_args; ++i) {
	if (i == 0)
	    printf "  "
	else if (i % 4 == 0)
	    printf ",\n  "
	else
	    printf ", "
	printf "IX86_BT_" func_types[i]
    }
    print "\n};\n\n"

    print "static const unsigned short ix86_builtin_func_start[] = {"
    for (i = 0; i < func_defs; ++i) {
	if (i == 0)
	    printf " "
	else if (i % 10 == 0)
	    printf "\n "
	printf " " func_start[i] ","
    }
    print " " func_args "\n};\n\n"

    print "static const enum ix86_builtin_func_type " \
	"ix86_builtin_func_alias_base[] = {"
    for (i = 0; i < alias_defs; ++i) {
	if (i == 0)
	    printf "  "
	else
	    printf ",\n  "
	printf alias_base[i]
    }
    print "\n};"
}
