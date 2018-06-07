/* G.Barrand : 
   put in a standalone file to rm of cpp of cxx on OSF1
   because of the '$' used in the name of variables.
*/
   
      

#  define F77_CHAR_ARG_PTR(s) ConCat(s,_ptr)
#  define F77_CHAR_ARG_LEN(s)
#  define F77_CHAR_ARG_DCL(s) struct dsc$descriptor_s *ConCat(s,_ptr);
#  define F77_CHAR_ARG_DEF(s) char *s = ConCat(s,_ptr)->dsc$a_pointer; \
                   int ConCat(len_,s) = ConCat(s,_ptr)->dsc$w_length;

#  define F77_CHAR_DEF_DSC(s,p,l) struct dsc$descriptor_s ConCat(s,_dsc);
#  define F77_CHAR_ASS_DSC(s,p,l) ConCat(s,_dsc).dsc$w_length = l; \
                                  ConCat(s,_dsc).dsc$b_dtype = DSC$K_DTYPE_T;\
                                  ConCat(s,_dsc).dsc$b_class = DSC$K_CLASS_S;\
                                  ConCat(s,_dsc).dsc$a_pointer = (char*)p;
#  define F77_CHAR_USE_PTR(s,p,l) &ConCat(s,_dsc)
#  define F77_CHAR_USE_LEN(s,p,l)


