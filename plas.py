#!/usr/bin/env python


OCT = 'oct'
OCTDIFF = 'octdiff'
FLOAT = 'float'
INT = 'int'

DEFAULT_GEN_VALUE = 100
DEFAULT_NUM_VALS = 10
DEFAULT_INC_CHANCE = 0.75
DEFAULT_MUT_CHANCE = 0.0


class BaseRandom(object):
	def __init__(self, seed = None, randg = None):
		import random
		self.seed = seed
		if randg:
			self.randg = randg
		else:
			try:
				if seed:
					self.randg = random.WichmannHill(seed)
				else:
					self.randg = random.WichmannHill()
			except:
				if seed:
					self.randg = random.SystemRandom(seed)
				else:
					self.randg = random.SystemRandom()
	
	def shuffle(self, obj):
		return self.randg.shuffle(obj)
		

class IntRandom(BaseRandom):
	def __call__(self, a, b):
		return int(self.randg.randint(a, b))


class FloatiRandom(BaseRandom):
	def __call__(self, a, b):
		return float(self.randg.randint(a, b))


class FloatRandom(BaseRandom):
	def __call__(self, a, b):
		return self.randg.uniform(a, b)


def float_rand(a, b):
	import random
	return random.uniform(a, b)


def int_rand(a, b):
	import random
	return int(random.randint(a, b))


def floati_rand(a, b):
	import random
	return float(random.randint(a, b))


def cons_a(cons):
	return cons[0]


def cons_b(cons):
	return cons[1]


def cons_d(cons):
	return cons[2]


def cons_single_var(cons):
	var = 0
	if cons and len(cons) == 3:
		if cons[0] == 0:
			if cons[1] != 0:
				var = cons[1]
		else:
			if cons[1] == 0:
				var = cons[0]
	return var


def valid_cons(cons):
	return (cons
		and len(cons) == 3
		and (cons[0] or cons[1])
	)


def norm_var(var):
	return var < 0 and -var or var


def transform_var(var):
	return (var != 0
		and (var < 0
			and norm_var(var + 1) * 2 + 2
			or  (var - 1) * 2 + 1
		)
		or 0
	)


def switch_var(var):
	var = norm_var(var)
	return (var != 0
		and (var % 2 == 0
			and var - 1
			or var + 1
		)
		or 0
	)


def split(cons):
	ca = []
	cb = []

	single_var = cons_single_var(cons)

	if single_var != 0:
		new_var = transform_var(single_var)
		#pos_var = single_var < 0 and new_var - 1 or new_var
		#neg_var = single_var < 0 and new_var     or new_var + 1
		ca.append(new_var)
		if single_var < 0:
			ca.append(new_var - 1)
		else:
			ca.append(new_var + 1)
		ca.append(2 * cons[2])

	else:
		a = cons[0]
		b = cons[1]
		ca.extend([0, 0, 0])
		cb.extend([0, 0, 0])

		if a < 0 and b > 0:
			a, b = b, a

		newa = transform_var(a);
		newb = transform_var(b);
		posa = a < 0 and newa - 1 or newa
		nega = a < 0 and newa     or newa + 1
		posb = b < 0 and newb - 1 or newb
		negb = b < 0 and newb     or newb + 1
		ca[2] = cons[2]
		cb[2] = cons[2]

		if a > 0 and b > 0:
			ca[0] = posa
			ca[1] = negb
			cb[0] = posb
			cb[1] = nega

		elif a > 0 and b < 0:
			ca[0] = posa
			ca[1] = posb
			cb[0] = negb
			cb[1] = nega

		elif a < 0 and b < 0:
			ca[0] = nega
			ca[1] = posb
			cb[0] = negb
			cb[1] = posa

		else:
			pass
			# One of them is zero!
	
	return [tuple(ca), tuple(cb)]


class Problem(object):
	def __init__(self):
		self.clear()


	def clear(self):
		self.constraints = {}
		self.generated_values = []
		self.problem_type = None
		self.nvars = None
		self.nconstraints = None
		self.varnames = {}
		self.original_constraints = {}
		self.valuetype = None
		self.sat = None
		self.actions = None


	def as_dict(self, include_all = True):
		import copy
		obj = {}
		if include_all or self.constraints     : obj['constraints'] = copy.deepcopy(self.constraints)
		if include_all or self.generated_values: obj['generated_values'] =  copy.deepcopy(self.generated_values)
		if include_all or self.problem_type    : obj['problem_type'] =  copy.deepcopy(self.problem_type)
		if include_all or self.nvars           : obj['nvars'] =  copy.deepcopy(self.nvars)
		if include_all or self.nconstraints    : obj['nconstraints'] =  copy.deepcopy(self.nconstraints)
		if include_all or self.varnames        : obj['varnames'] =  copy.deepcopy(self.varnames)
		if include_all or self.original_constraints: obj['original_constraints'] =  copy.deepcopy(self.original_constraints)
		if include_all or self.valuetype       : obj['valuetype'] =  copy.deepcopy(self.valuetype)
		if include_all or self.sat             : obj['sat'] = copy.deepcopy(self.sat)
		if include_all or self.actions         : obj['actions'] = copy.deepcopy(self.actions)
		return obj


	def __repr__(self):
		import pprint
		return repr(self.as_dict(include_all = True))


	def __str__(self):
		import pprint
		return pprint.pformat(self.as_dict(include_all = False))


	def restore_cons(self, cons):
		vals = self.constraints.get(cons[0], None)
		if vals is None:
			vals = self.constraints[cons[0]] = {}
		vals = vals.get(cons[1], None)
		if vals is None:
			vals[cons[1]] = {}
		vals[cons[1]] = cons[2]


	def restore(self, actions):
		for cons in actions.get('removed', []): self.restore_cons(cons)
		for cons in actions.get('mutated', []): self.restore_cons(cons)


	def recount(self):
		self.nconstraints = 0
		for a in self.constraints:
			self.nconstraints += len(self.constraints[a])


	def cons_list(self, ret = None):
		conss = ret or []
		for a in self.constraints:
			for b in self.constraints[a]:
				conss.append((a, b, self.constraints[a][b]))
		return conss


	def cons_iter(self):
		for a in self.constraints:
			for b in self.constraints[a]:
				yield((a, b, self.constraints[a][b]))


	def cons_len(self):
		plen = self.nconstraints
		if plen is None:
			self.recount_cons()
			plen = self.nconstraints
		return plen


	def save(self, out = None, filename = None, nconstraints = None):
		import sys

		if filename:
			out = open(filename, 'w')
		elif out is None:
			out = sys.stdout

		nconstraints = nconstraints or self.nconstraints
		problem_type = self.problem_type

		if self.valuetype == 'int':
			problem_type = "%s_int" % problem_type

		if self.sat is True:
			out.write("# Should be SATISFIABLE (probably consistent)\n")
		elif self.sat is False:
			out.write("# Should be UNSATISFIABLE (probably inconsistent)\n")
		else:
			out.write("# Not sure if SAT or UNSAT (no clue about consistency)\n")

		if problem.actions:
			out.write("# %d constraint(s) removed, %d constraint(s) mutated.\n" %
				(len(problem.actions['removed']), len(problem.actions['mutated']))
			)

		out.write("p %s %d %d\n" % (problem_type, self.nvars, nconstraints))

		for a in self.constraints:
			for b in self.constraints[a]:
				if nconstraints < 0:
					raise "Problem has more constraints than the declared value."
				val = (self.valuetype == 'int' and "%d" or "%g") % self.constraints[a][b]
				if b == 0:
					out.write("e 0 %d %s\n" % (a, val))
				else:
					out.write("e %d %d %s\n" % (a, b, val))
				nconstraints -= 1

		for v in self.varnames:
			out.write("n %d %s\n" % (v, self.varnames[v]))

		if self.generated_values:
			out.write("# -- Generated values:\n")
			for tpl in generated_values:
				out.write("# values %s\n" % repr(tpl))
		
		if self.actions:
			removed = (self.actions.get('removed', []))
			mutated = (self.actions.get('mutated', []))
			if removed or mutated:
				out.write("# -- Modified octagonal constraints\n")
			if removed:
				for tpl in removed:
					out.write("# removed %s\n" % repr(tpl))
			if mutated:
				for tpl in mutated:
					out.write("# mutated %s\n" % repr(tpl))
			
		if filename:
			out.close()


	def generate_oct(self, nvars, nvals = DEFAULT_NUM_VALS, min_value = 0, max_value = DEFAULT_GEN_VALUE, randg = None, var_prefix = 'x'):
		import sys
		import numbers
		
		generated_values = []
		self.clear()
		self.problem_type = OCT
		self.nvars = nvars
		self.nconstraints = 2 * nvars * (2 * (nvars - 1) + 1)
		self.sat = True
		
		if randg is None:
			randg = float_rand
			
		# First, generate the values
		for val_idx in range(nvals):
			vec = []
			
			# First, generate the values
			for var_idx in range(nvars):
				var = var_idx + 1
				var_value = randg(min_value, max_value)
				vec.append(var_value)
				
				# Keep the maximum value for each variable updated
				if  var not in self.constraints: self.constraints[ var] = {}
				if -var not in self.constraints: self.constraints[-var] = {}
				self.constraints[var][0] = max(
					0 in self.constraints[var]
						and self.constraints[var][0]
						or var_value,
					var_value
				)
				self.constraints[-var][0] = max(
					0 in self.constraints[-var]
						and self.constraints[-var][0]
						or -var_value,
					-var_value
				)
			
			# Add the generated tuple to the stats object
			generated_values.append(tuple(vec))
			
			# Generate constraints
			for var1_idx in range(nvars):
				var1 = var1_idx + 1

				for var2_idx in range(nvars):
					var2 = var2_idx + 1

					if var1 != var2:
						val_pa_pb = vec[var1_idx] + vec[var2_idx]
						self.constraints[var1][var2] = max(
							var2 in self.constraints[var1]
								and self.constraints[var1][var2]
								or val_pa_pb,
							val_pa_pb
						)

						val_pa_nb = vec[var1_idx] - vec[var2_idx]
						self.constraints[var1][-var2] = max(
							-var2 in self.constraints[var1]
								and self.constraints[var1][-var2]
								or val_pa_nb,
							val_pa_nb
						)

						val_na_pb = -vec[var1_idx] + vec[var2_idx]
						self.constraints[-var1][var2] = max(
							var2 in self.constraints[-var1]
								and self.constraints[-var1][var2]
								or val_na_pb,
							val_na_pb
						)

						val_na_nb = -vec[var1_idx] - vec[var2_idx]
						self.constraints[-var1][-var2] = max(
							-var2 in self.constraints[-var1]
								and self.constraints[-var1][-var2]
								or val_na_nb,
							val_na_nb
						)
		
		for var_idx in range(nvars):
			self.varnames[var_idx + 1] = "%s%d" % (var_prefix, var_idx + 1)

		if isinstance(generated_values[0][0], numbers.Integral):
			self.valuetype = INT
		else:
			self.valuetype = FLOAT

		return generated_values


	def mutate(self, randg = float_rand, mul_randg = float_rand, inc_chance = DEFAULT_INC_CHANCE, mut_chance = DEFAULT_MUT_CHANCE, max_removals = None, max_mutations = None):
		import random
		
		actions = {
			'removed': [],
			'mutated': [],
		}
		
		if max_removals is None:
			max_removals = self.nconstraints // 2
		max_inclusions = self.nconstraints - max_removals
		if max_mutations is None:
			max_mutations = max_inclusions // 2
		elif max_mutations > max_inclusions:
			max_mutations = max_inclusions
		
		num_removals = 0
		num_mutations = 0
		
		def rand():
			return randg(0, 1)
		
		cons_set = []
		for a in self.constraints:
			for b in self.constraints[a]:
				cons_set.append((a, b, self.constraints[a][b]))
		random.shuffle(cons_set, rand)
		cons_set = cons_set[0:max_inclusions]
		
		def act():
			for cons in cons_set:
				if num_removals >= max_removals and num_mutations >= max_mutations:
					break
				else:
					yield cons[0], cons[1], cons[2]
		
		for a, b, d in act():
			chance = randg(0, 1)
			# Not removing
			if chance < inc_chance:
				# Only (possibly) mutate if the constraint is not going to be removed 
				if num_mutations < max_mutations and chance < mut_chance:
					nd = mul_randg(0, 1) * d
					if nd != d:
						actions['mutated'].append((a, b, d))
						self.sat = False
						d = self.constraints[a][b] = nd
						num_mutations += 1
			# Removing
			else:
				# Should I declare the self. UNSAT if I remove a constraint?
				# Am I right when assuming solvers are able to rediscover deleted constraints?
				actions['removed'].append((a, b, d))
				num_removals += 1

		for cons in actions['removed']:
			self.nconstraints -= 1
			del self.constraints[cons[0]][cons[1]]

		return actions


	def convert(self, new_problem_type, var_prefix = 'y'):
		if self.problem_type == OCT and new_problem_type == OCTDIFF:
			return self._convert_oct_octdiff(var_prefix = var_prefix)
		return None


	def _convert_oct_octdiff(self, var_prefix = 'y'):
		new_problem = Problem()

		new_problem.nvars = self.nvars * 2
		new_problem.problem_type = OCTDIFF
		new_problem.valuetype = self.valuetype
		new_problem.sat = self.sat

		for cons in self.cons_iter():
			diff_conss = split(cons)
			for diff_cons in diff_conss:
				if valid_cons(diff_cons):
					# Original constraint information
					if not diff_cons[0] in new_problem.original_constraints:
						new_problem.original_constraints[diff_cons[0]] = {}
					new_problem.original_constraints[diff_cons[0]][diff_cons[1]] = cons
					# New constraint insertion
					if not diff_cons[0] in new_problem.constraints:
						new_problem.constraints[diff_cons[0]] = {}
					new_problem.constraints[diff_cons[0]][diff_cons[1]] = diff_cons[2]
				#	print("Inserted conversion %r for constraint %r" % (diff_cons, cons))
				#else:
				#	print("Generated invalid conversion %r for constraint %r" % (diff_cons, cons))
		
		new_problem.recount()

		for var_idx in range(self.nvars):
			var = var_idx + 1
			varname = self.varnames[var]
			new_var = transform_var(var)
			pos_var = var < 0 and new_var - 1 or new_var
			neg_var = var < 0 and new_var     or new_var + 1
			new_pos_varname = "%s%d__p_%s" % (var_prefix, pos_var, varname)
			new_neg_varname = "%s%d__n_%s" % (var_prefix, neg_var, varname)
			new_problem.varnames[pos_var] = new_pos_varname
			new_problem.varnames[neg_var] = new_neg_varname

		return new_problem


if __name__ == "__main__":
	import os, sys
	import argparse
	parser = argparse.ArgumentParser(
		prog = 'PLAS',
		description = "An octagon domain problem generator and helper tool.",
	)
	parser.add_argument(
		'--version',
		action = 'version',
		version = "%(prog)s 0.1, " + parser.description,
	)
	parser.add_argument(
		'command',
		type = str,
		choices = ('gen', ),
		help = "The command to execute. The only command available as of now is "+
			"'gen', that generates one or more random octagon domain-based problems.",
		default = 'gen',
	)
	parser.add_argument(
		'--nvars',
		type = int,
		help = "Specify the number of variables for each generated problem.",
		default = 3,
	)
	parser.add_argument(
		'--nvals',
		type = int,
		help = "The number of variable values' tuples to generate, e.g. " +
			"`--nvars=3 --nvals=500` indicates that problems will be constructed " +
			"from 500 random tuples of size 3, where each tuple index represents a variable.",
		default = DEFAULT_NUM_VALS,
	)
	parser.add_argument(
		'--min',
		type = int,
		help = "Specifies the minimum value of the generated values.",
		default = 0,
	)
	parser.add_argument(
		'--max',
		type = int,
		help = "Specifies the maximum value of the generated values.",
		default = DEFAULT_GEN_VALUE,
	)
	parser.add_argument(
		'--nproblems',
		type = int,
		help = "Specifies the number of octagon problems to generate.",
		default = 1,
	)
	parser.add_argument(
		'--nproblems-startnum',
		type = int,
		help = "Starting number for outputted problems. Must be greater than 0.",
		default = 1,
	)
	parser.add_argument(
		'--nproblems-total',
		type = int,
		help = "The total number of problems to be generated on a multi-call session. " +
			"Should be used by automated scripts, so that PLAS generates correctly-padded " +
			"problem IDs on filenames (e.g. 001-950 for 950 problems created from multiple " +
			"calls to the application).",
		default = None,
	)
	parser.add_argument(
		'-o',
		'--output',
		type = str,
		help = "Specifies the output file. " +
			"If not given, defaults to STDOUT when NPROBLEMS equals 1, else " +
			"the pattern 'problem-@s-@n.plas' is used. To force output to " +
			"STDOUT, '-' can be specified (as is), but if NPROBLEMS > 1, the generated " +
			"problems will be separated by a NUL character. Output can be a pattern " +
			"consisting of two special characters: '@n' (for the problem index/number), " +
			"'@s' (for the problem's expected consistency/satisfiability) and " +
			"'@t' for the problem's type.",
		default = None,
	)
	parser.add_argument(
		'--open-mode',
		type = str,
		choices = ('w', 'a'),
		help = "Specifies the mode to be used on file opening. " +
			"Used only on single-problem generation (NPROBLEMS == 1).",
		default = 'w',
	)
	parser.add_argument(
		'-S',
		'--seed',
		type = int,
		help = "Seed to be used on the random numbers generated for problem construction.",
		default = None,
	)
	parser.add_argument(
		'-T',
		'--problem-type',
		type = str,
		choices = ('oct', 'diff', 'octdiff'),
		help = "Specifies type of problems to generate. 'oct' generates problems described "+
			"as octagonal constraints. 'diff' generates octagon domain-based problems " +
			"converted to a difference-based system. 'octdiff' outputs the original 'oct' " +
			"problem and its transformed 'diff' counterpart. Note that 'octdiff' will generate " + 
			"twice the amount of problems.",
		default = 'oct',
	)
	parser.add_argument(
		'--value-type',
		type = str,
		choices = ('f', 'i', 'fi'),
		help = "Specifies the value type to be used. " +
			"Can be any of 'f' (float), 'i' (integer) " +
			"or 'fi' (floor'd float).",
		default = 'f',
	)
	parser.add_argument(
		'--var-prefix',
		type = str,
		help = "Specifies the prefix for generated variables.",
		default = 'x',
	)
	parser.add_argument(
		'--tvar-prefix',
		type = str,
		help = "Specifies the prefix for transformed variables. Applicable only when PROBLEM_TYPE == 'octdiff'.",
		default = 'y',
	)
	parser.add_argument(
		'--no-modification',
		action = 'store_const',
		const = True,
		help = "If given, generated problems will be fully complete, meaning no removal or mutation of " +
			"constraints.",
		default = False,
	)
	parser.add_argument(
		'--inclusion-seed',
		type = int,
		help = "Specifies the seed for random inclusion's number generator.",
		default = None,
	)
	parser.add_argument(
		'--mutation-seed',
		type = int,
		help = "Specifies the seed for random mutation's number generator.",
		default = None,
	)
	parser.add_argument(
		'--mutation-chance',
		type = float,
		help = "Percentage chance for transforming the constant in a constraint. "+
			"Must be between 0 (never) and 1 (always). Note that, if mutated, the resulting constraint " +
			"will be incorrect, hence the whole problem MAY become unsatisfiable afterwards. " + 
			"The default is 0." +
			"Mutation always happen at the 'oct' generation stage (e.g. transformation happens adter mutation).",
		default = DEFAULT_MUT_CHANCE,
	)
	parser.add_argument(
		'--inclusion-chance',
		type = float,
		help = "Percentage chance for including the generated constraints into the " +
			"resulting problem. Must be between 0 (never include) or 1 " + 
			"(always include). The default is 0.75. " +
			"Inclusion always happen at the 'oct' generation stage (e.g. transformation happens adter mutation).",
		default = DEFAULT_INC_CHANCE,
	)

	args = parser.parse_args()

	if args.command == 'gen':
		if args.value_type == 'f':
			randg = FloatRandom(seed = args.seed)
		elif args.value_type == 'i':
			randg = IntRandom(seed = args.seed)
		else:
			randg = FloatiRandom(seed = args.seed)

		mut_randg = FloatRandom(args.mutation_seed)
		inc_randg = FloatRandom(args.inclusion_seed)
		ndigits = len(str(args.nproblems))
		problems = []

		if args.nproblems_startnum <= 0:
			sts.stderr.write("ERROR: NPROBLEMS_STARTNUM should be greater than 0.")
			exit(1)

		num_problems = args.nproblems
		if args.problem_type == 'octdiff':
			num_problems *= 2

		if args.nproblems_total is None:
			args.nproblems_total = num_problems + (args.nproblems_startnum - 1)
		
		if num_problems < 1:
			sys.stderr.write("ERROR: nproblems should be greater than or equal to 1.\n")
			exit(1)
		elif num_problems == 1:
			if not args.output:
				args.output = "-" #"problem-%%.plas"
		else:
			if args.output:
				if args.output.find('@n') < 0:
					sys.stderr.write("ERROR: A problem no. insertion char '@n' must be provided on the output filename if nproblems > 1.\n")
					exit(1)
			else:
				args.output = "problem-@s-@n.@t.plas"

		for problem_idx in range(args.nproblems):
			problem_id = problem_idx + 1
			actions = None
			problem = Problem()
			problem_diff = None

			generated_values = problem.generate_oct(
				args.nvars,
				nvals = args.nvals,
				min_value = args.min,
				max_value = args.max,
				randg = randg,
				var_prefix = args.var_prefix,
			)

			if not args.no_mutation:
				actions = problem.mutate(
					randg = inc_randg,
					mul_randg = mut_randg,
					inc_chance = args.inclusion_chance,
					mut_chance = args.mutation_chance,
				)

			if args.problem_type in ('octdiff', 'diff'):
				problem_diff = problem.convert(OCTDIFF, var_prefix = args.tvar_prefix)

			problem.actions = actions
			problem.generated_values = generated_values
			if problem_diff:
				problem_diff.actions = actions
				problem_diff.generated_values = generated_values

			if args.problem_type == 'oct':
				problems.append((problem,))
			elif args.problem_type == 'diff':
				problems.append((problem_diff,))
			else:
				problems.append((problem, problem_diff))

		if num_problems == 1:
			sat = 'undef'
			if problems[0][0].sat is True:
				sat = 'sat'
			elif problems[0][0].sat is False:
				sat = 'unsat'
			filename = args.output.replace('@s', sat)
			filename = args.output.replace('@t', problems[0][0].problem_type)
			if filename != "-":
				f = open(filename, args.open_mode)
				problems[0][0].save(out = f)
				f.close()
			else:
				problems[0][0].save(out = sys.stdout)
		else:
			sep = ""
			num_problems = len(problems)
			ndigits = len(str(args.nproblems_total))
			for problem_idx in range(num_problems):
				problem_tuple = problems[problem_idx]
				problem_id = problem_idx + args.nproblems_startnum

				for problem in problem_tuple:
					sat = 'undef'
					if problem.sat is True:
						sat = 'sat'
					elif problem.sat is False:
						sat = 'unsat'
					num_str = ("{0:0%dd}" % (ndigits)).format(problem_id)
					filename = args.output.replace('@s', sat).replace('@n', num_str).replace('@t', problem.problem_type)
					if filename == "-":
						sys.stdout.write(sep)
						problem.save(out = sys.stdout)
						sep = "\0"
					else:
						problem.save(filename = filename)
	else:
		sys.stderr.write("ERROR: Unknown command.\n")
		exit(1)

