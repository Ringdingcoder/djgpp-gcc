/* Support routines for Value Range Propagation (VRP).
   Copyright (C) 2016-2017 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_TREE_VRP_H
#define GCC_TREE_VRP_H

/* Type of value ranges.  See value_range below for a
   description of these types.  */
enum value_range_type { VR_UNDEFINED, VR_RANGE,
			VR_ANTI_RANGE, VR_VARYING, VR_LAST };

/* Range of values that can be associated with an SSA_NAME after VRP
   has executed.  */
struct GTY((for_user)) value_range
{
  /* Lattice value represented by this range.  */
  enum value_range_type type;

  /* Minimum and maximum values represented by this range.  These
     values should be interpreted as follows:

	- If TYPE is VR_UNDEFINED or VR_VARYING then MIN and MAX must
	  be NULL.

	- If TYPE == VR_RANGE then MIN holds the minimum value and
	  MAX holds the maximum value of the range [MIN, MAX].

	- If TYPE == ANTI_RANGE the variable is known to NOT
	  take any values in the range [MIN, MAX].  */
  tree min;
  tree max;

  /* Set of SSA names whose value ranges are equivalent to this one.
     This set is only valid when TYPE is VR_RANGE or VR_ANTI_RANGE.  */
  bitmap equiv;
};

extern void vrp_intersect_ranges (value_range *vr0, value_range *vr1);
extern void vrp_meet (value_range *vr0, const value_range *vr1);
extern void dump_value_range (FILE *, const value_range *);
extern void extract_range_from_unary_expr (value_range *vr,
					   enum tree_code code,
					   tree type,
					   value_range *vr0_,
					   tree op0_type);

extern bool vrp_operand_equal_p (const_tree, const_tree);

struct assert_info
{
  /* Predicate code for the ASSERT_EXPR.  Must be COMPARISON_CLASS_P.  */
  enum tree_code comp_code;

  /* Name to register the assert for.  */
  tree name;

  /* Value being compared against.  */
  tree val;

  /* Expression to compare.  */
  tree expr;
};

extern void register_edge_assert_for (tree, edge, enum tree_code,
				      tree, tree, vec<assert_info> &);
extern bool stmt_interesting_for_vrp (gimple *);
extern void set_value_range_to_varying (value_range *);
extern int range_includes_zero_p (tree, tree);
extern bool infer_value_range (gimple *, tree, tree_code *, tree *);

#endif /* GCC_TREE_VRP_H */
