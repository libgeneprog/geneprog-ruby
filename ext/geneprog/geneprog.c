#include <ruby.h>

#include "geneprog/gene.h"
#include "geneprog/cgp-data.h"

static void cgpgene_deallocate(void * gene)
{
  GP_CGP_free((GP_Gene *)gene);
}

static VALUE cgpgene_allocate(VALUE klass)
{
  GP_Gene* gene = malloc(sizeof(GP_Gene));
  return Data_Wrap_Struct(klass, NULL, cgpgene_deallocate, gene);
}

static VALUE cgpgene_initialize(VALUE self, VALUE vNumIn, VALUE vNumMid, VALUE vNumOut)
{
  Check_Type(vNumIn, T_FIXNUM);
  Check_Type(vNumMid, T_FIXNUM);
  Check_Type(vNumOut, T_FIXNUM);
  unsigned int num_in = NUM2UINT(vNumIn);
  unsigned int num_mid = NUM2UINT(vNumMid);
  unsigned int num_out= NUM2UINT(vNumOut);

  GP_Gene* gene;

  Data_Get_Struct(self, GP_Gene, gene);

  GP_CGP_init(gene, num_in, num_mid, num_out);

  rb_iv_set(self, "@num_inputs", vNumIn);
  rb_iv_set(self, "@num_outputs", vNumOut);

  return self;
}

static VALUE cgpgene_evaluate(VALUE self, VALUE vInputs)
{
  Check_Type(vInputs, T_ARRAY);
  // Convert our inputs:
  VALUE vNumIn = rb_ary_length(vInputs);
  unsigned int num_in = NUM2UINT(vNumIn);
  // Make a c array:
  double *inputs = (double *)malloc(sizeof(double)*num_in); 
  for(int i=0; i<num_in; i++){
    VALUE elem = rb_ary_entry(vInputs, i);
    inputs[i] = NUM2DBL(elem);
  }

  // Make a c array to hold the outputs:
  VALUE vNumOut = rb_iv_get(self, "@num_outputs");
  unsigned int num_out = NUM2UINT(vNumOut);
  double *outputs = (double *)malloc(sizeof(double)*num_out); 

  // Get our gene:
  GP_Gene* gene;
  Data_Get_Struct(self, GP_Gene, gene);

  // Call evaluate:
  gene->evaluate(inputs, outputs, gene->data);

  // Make a new array to hold the result:
  VALUE result = rb_ary_new();
  for(int i=0; i<num_out; i++){
    rb_ary_push(result, DBL2NUM(outputs[i]));
  }

  // Clean up:
  free(inputs);
  free(outputs);

  return result;

}

void Init_geneprog(void) {
  VALUE mGeneprog = rb_define_module("Geneprog");

  VALUE cCGPGene = rb_define_class_under(mGeneprog, "CGPGene", rb_cObject);
  rb_define_alloc_func(cCGPGene, cgpgene_allocate);
  rb_define_method(cCGPGene, "initialize", cgpgene_initialize, 3);
  //rb_define_method(cCGPGene, "evaluate", cgpgene_evaluate, 1);
}
