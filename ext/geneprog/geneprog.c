// SPDX-License-Identifier: BSD-3-Clause
#include <ruby.h>

#include "geneprog/gene.h"
#include "geneprog/cgp-data.h"

static void cgpgene_deallocate(void *gene)
{
	GP_CGP_free((struct GP_Gene *)gene);
}

static VALUE cgpgene_allocate(VALUE klass)
{
	struct GP_Gene *gene;

	gene = malloc(sizeof(struct GP_Gene));
	return Data_Wrap_Struct(klass, NULL, cgpgene_deallocate, gene);
}

static VALUE cgpgene_initialize(VALUE self,
				VALUE vNumIn,
				VALUE vNumMid,
				VALUE vNumOut)
{
	Check_Type(vNumIn, T_FIXNUM);
	Check_Type(vNumMid, T_FIXNUM);
	Check_Type(vNumOut, T_FIXNUM);

	unsigned int num_in, num_mid, num_out;
	struct GP_Gene *gene;

	num_in = NUM2UINT(vNumIn);
	num_mid = NUM2UINT(vNumMid);
	num_out = NUM2UINT(vNumOut);

	Data_Get_Struct(self, struct GP_Gene, gene);

	GP_CGP_init(gene, num_in, num_mid, num_out);

	// Randomize
	GP_CGP_randomize(gene);

	rb_iv_set(self, "@num_inputs", vNumIn);
	rb_iv_set(self, "@num_outputs", vNumOut);

	return self;
}

static VALUE cgpgene_evaluate(VALUE self, VALUE vInputs)
{
	Check_Type(vInputs, T_ARRAY);
	long num_in;
	double *inputs, *outputs;
	unsigned int num_out;
	VALUE vNumOut, elem, result;
	struct GP_Gene *gene;

	// Convert our inputs:
	num_in = RARRAY_LEN(vInputs);
	// Make a c array:
	inputs = (double *)malloc(sizeof(double) * num_in);
	for (int i = 0; i < num_in; i++) {
		elem = rb_ary_entry(vInputs, i);
		inputs[i] = NUM2DBL(elem);
	}

	// Make a c array to hold the outputs:
	vNumOut = rb_iv_get(self, "@num_outputs");
	num_out = NUM2UINT(vNumOut);
	outputs = (double *)malloc(sizeof(double) * num_out);

	// Get our gene:
	Data_Get_Struct(self, struct GP_Gene, gene);

	// Call evaluate:
	gene->evaluate(inputs, outputs, gene->data);

	// Make a new array to hold the result:
	result = rb_ary_new();
	for (int i = 0; i < num_out; i++)
		rb_ary_push(result, DBL2NUM(outputs[i]));

	// Clean up:
	free(inputs);
	free(outputs);

	return result;
}

void Init_geneprog(void)
{
	VALUE mGeneprog, cCGPGene;

	mGeneprog = rb_define_module("Geneprog");

	cCGPGene = rb_define_class_under(mGeneprog, "CGPGene", rb_cObject);
	rb_define_alloc_func(cCGPGene, cgpgene_allocate);
	rb_define_method(cCGPGene, "initialize", cgpgene_initialize, 3);
	rb_define_method(cCGPGene, "evaluate", cgpgene_evaluate, 1);
}
