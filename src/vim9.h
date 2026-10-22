/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9.h: types and globals used for Vim9 script.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# include <float.h>
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    ISN_EXEC,	    // execute Ex command line isn_arg.string
    ISN_EXECCONCAT, // execute Ex command from isn_arg.number items on stack
    ISN_EXEC_ವಿಭಜಿಸು, // execute Ex command from isn_arg.string split at NL
    ISN_EXECRANGE,  // execute EX command that is only a range
    ISN_LEGACY_EVAL, // evaluate expression isn_arg.string with legacy syntax.
    ISN_ECHO,	    // :echo with isn_arg.echo.echo_count items on top of stack
    ISN_EXECUTE,    // :execute with isn_arg.number items on top of stack
    ISN_ECHOMSG,    // :echomsg with isn_arg.number items on top of stack
    ISN_ECHOCONSOLE, // :echoconsole with isn_arg.number items on top of stack
    ISN_ECHOWINDOW, // :echowindow with isn_arg.number items on top of stack
    ISN_ECHOERR,    // :echoerr with isn_arg.number items on top of stack
    ISN_RANGE,	    // compute range from isn_arg.string, push to stack
    ISN_SUBSTITUTE, // :s command with expression

    ISN_ಮೂಲ,	    // source autoload script, isn_arg.number is the script ID
    ISN_INSTR,	    // instructions compiled from expression
    ISN_CONSTRUCT,  // construct an object, using construct_T
    ISN_GET_OBJ_MEMBER, // object member, index is isn_arg.number
    ISN_GET_ITF_MEMBER, // interface member, index is isn_arg.classmember
    ISN_STORE_THIS, // store value in "this" object member, index is
		    // isn_arg.number
    ISN_ತುಂಬು_CLASSMEMBER,  // load class member, using isn_arg.classmember
    ISN_STORE_CLASSMEMBER,  // store in class member, using isn_arg.classmember

    // get and set variables
    ISN_ತುಂಬು,	    // push local variable isn_arg.number
    ISN_LOADV,	    // push v: variable isn_arg.number
    ISN_LOADG,	    // push g: variable isn_arg.string
    ISN_LOADAUTO,   // push g: autoload variable isn_arg.string
    ISN_LOADB,	    // push b: variable isn_arg.string
    ISN_LOADW,	    // push w: variable isn_arg.string
    ISN_LOADT,	    // push t: variable isn_arg.string
    ISN_LOADGDICT,  // push g: dict
    ISN_LOADBDICT,  // push b: dict
    ISN_LOADWDICT,  // push w: dict
    ISN_LOADTDICT,  // push t: dict
    ISN_LOADS,	    // push s: variable isn_arg.loadstore
    ISN_LOADEXPORT, // push exported variable isn_arg.loadstore
    ISN_LOADOUTER,  // push variable from outer scope isn_arg.outer
    ISN_LOADSCRIPT, // push script-local variable isn_arg.script.
    ISN_LOADOPT,    // push option isn_arg.string
    ISN_LOADENV,    // push environment variable isn_arg.string
    ISN_LOADREG,    // push register isn_arg.number

    ISN_STORE,	    // pop into local variable isn_arg.number
    ISN_STOREV,	    // pop into v: variable isn_arg.number
    ISN_STOREG,	    // pop into global variable isn_arg.string
    ISN_STOREAUTO,  // pop into global autoload variable isn_arg.string
    ISN_STOREB,	    // pop into buffer-local variable isn_arg.string
    ISN_STOREW,	    // pop into window-local variable isn_arg.string
    ISN_STORET,	    // pop into tab-local variable isn_arg.string
    ISN_STORES,	    // pop into script variable isn_arg.loadstore
    ISN_STOREEXPORT, // pop into exported script variable isn_arg.loadstore
    ISN_STOREOUTER,  // pop variable into outer scope isn_arg.outer
    ISN_STORESCRIPT, // pop into script variable isn_arg.script
    ISN_STOREOPT,    // pop into option isn_arg.storeopt
    ISN_STOREFUNCOPT, // pop into option isn_arg.storeopt
    ISN_STOREENV,    // pop into environment variable isn_arg.string
    ISN_STOREREG,    // pop into register isn_arg.number
    // ISN_STOREOTHER, // pop into other script variable isn_arg.other.

    ISN_STORENR,    // store number into local variable isn_arg.storenr.stnr_idx
    ISN_STOREINDEX,	// store into list or dictionary, using
			// isn_arg.storeindex; value/index/variable on stack
    ISN_STORERANGE,	// store into blob,
			// value/index 1/index 2/variable on stack

    ISN_UNLET,		// unlet variable isn_arg.unlet.ul_name
    ISN_UNLETENV,	// unlet environment variable isn_arg.unlet.ul_name
    ISN_UNLETINDEX,	// unlet item of list or dict
    ISN_UNLETRANGE,	// unlet items of list

    ISN_LOCKUNLOCK,	// :lock and :unlock for local variable member
    ISN_LOCKCONST,	// lock constant value

    // constants
    ISN_PUSHNR,		// push number isn_arg.number
    ISN_PUSHBOOL,	// push bool value isn_arg.number
    ISN_PUSHSPEC,	// push special value isn_arg.number
    ISN_PUSHF,		// push float isn_arg.fnumber
    ISN_PUSHS,		// push string isn_arg.string
    ISN_PUSHBLOB,	// push blob isn_arg.blob
    ISN_PUSHFUNC,	// push func isn_arg.string
    ISN_PUSHCHANNEL,	// push NULL channel
    ISN_PUSHJOB,	// push NULL job
    ISN_PUSHOBJ,	// push NULL object
    ISN_PUSHCLASS,	// push class, uses isn_arg.classarg
    ISN_NEWLIST,	// push list from stack items, size is isn_arg.number
			// -1 for null_list
    ISN_NEWTUPLE,	// push tuple from stack items, size is isn_arg.number
			// -1 for null_list
    ISN_NEWDICT,	// push dict from stack items, size is isn_arg.number
			// -1 for null_dict
    ISN_NEWPARTIAL,	// push NULL partial

    ISN_AUTOLOAD,	// get item from autoload import, function or variable

    // function call
    ISN_BCALL,	    // call builtin function isn_arg.bfunc
    ISN_DCALL,	    // call def function isn_arg.dfunc
    ISN_METHODCALL, // call method on interface, uses isn_arg.mfunc
    ISN_UCALL,	    // call user function or funcref/partial isn_arg.ufunc
    ISN_PCALL,	    // call partial, use isn_arg.pfunc
    ISN_PCALL_ಕೊನೆ,  // cleanup after ISN_PCALL with cpf_top set
    ISN_RETURN,	    // return, result is on top of stack
    ISN_RETURN_VOID, // Push void, then return
    ISN_RETURN_OBJECT, // Push constructed object, then return
    ISN_FUNCREF,    // push a function ref to dfunc isn_arg.funcref
    ISN_NEWFUNC,    // create a global function from a lambda function
    ISN_DEF,	    // list functions
    ISN_DEFER,	    // :defer  argument count is isn_arg.number

    // expression operations
    ISN_JUMP,	    // jump if condition is matched isn_arg.jump
    ISN_JUMP_IF_ARG_ಗಣ, // jump if argument is already set, uses
			 // isn_arg.jumparg
    ISN_JUMP_IF_ARG_NOT_ಗಣ, // jump if argument is not set, uses
			 // isn_arg.jumparg

    // loop
    ISN_FOR,	    // get next item from a list, uses isn_arg.forloop
    ISN_WHILE,	    // jump if condition false, store funcref count, uses
		    // isn_arg.whileloop
    ISN_ENDLOOP,    // handle variables for closures, uses isn_arg.endloop

    ISN_TRY,	    // add entry to ec_trystack, uses isn_arg.tryref
    ISN_THROW,	    // pop value of stack, store in v:exception
    ISN_PUSHEXC,    // push v:exception
    ISN_CATCH,	    // drop v:exception
    ISN_FINALLY,    // start of :finally block
    ISN_ENDTRY,	    // take entry off from ec_trystack
    ISN_TRYCONT,    // handle :continue or :break inside a :try statement

    // more expression operations
    ISN_ADDLIST,    // add two lists
    ISN_ADDTUPLE,   // add two tuples
    ISN_ADDBLOB,    // add two blobs

    // operation with two arguments; isn_arg.op.op_type is exprtype_T
    ISN_OPNR,
    ISN_OPFLOAT,
    ISN_OPANY,

    // comparative operations; isn_arg.op.op_type is exprtype_T, op_ic used
    ISN_COMPAREBOOL,
    ISN_COMPARESPECIAL,
    ISN_COMPARENULL,
    ISN_COMPARENR,
    ISN_COMPAREFLOAT,
    ISN_COMPARESTRING,
    ISN_COMPAREBLOB,
    ISN_COMPARELIST,
    ISN_COMPARETUPLE,
    ISN_COMPAREDICT,
    ISN_COMPAREFUNC,
    ISN_COMPAREANY,
    ISN_COMPAREOBJECT,

    // expression operations
    ISN_CONCAT,     // concatenate isn_arg.number strings
    ISN_STRINDEX,   // [expr] string index
    ISN_STRSLICE,   // [expr:expr] string slice
    ISN_LISTAPPEND, // append to a list, like add()
    ISN_LISTINDEX,  // [expr] list index
    ISN_LISTSLICE,  // [expr:expr] list slice
    ISN_TUPLEINDEX,  // [expr] tuple index
    ISN_TUPLESLICE,  // [expr:expr] tuple slice
    ISN_BLOBINDEX,  // [expr] blob index
    ISN_BLOBSLICE,  // [expr:expr] blob slice
    ISN_ANYINDEX,   // [expr] runtime index
    ISN_ANYSLICE,   // [expr:expr] runtime slice
    ISN_ತುಂಡು,	    // drop isn_arg.number items from start of list
    ISN_BLOBAPPEND, // append to a blob, like add()
    ISN_GETITEM,    // push list item, isn_arg.number is the index
    ISN_MEMBER,	    // dict[member]
    ISN_STRINGMEMBER, // dict.member using isn_arg.string
    ISN_2BOOL,	    // falsy/truthy to bool, uses isn_arg.tobool
    ISN_COND2BOOL,  // convert value to bool
    ISN_2STRING,    // convert value to string at isn_arg.tostring on stack
    ISN_2STRING_ANY, // like ISN_2STRING but check type
    ISN_NEGATENR,   // apply "-" to number

    ISN_CHECKTYPE,  // check value type is isn_arg.type.ct_type
    ISN_CHECKLEN,   // check list length is isn_arg.checklen.cl_min_len
    ISN_SETTYPE,    // set dict type to isn_arg.type.ct_type

    ISN_CLEARDICT,  // clear dict saved by ISN_MEMBER/ISN_STRINGMEMBER
    ISN_USEDICT,    // use or clear dict saved by ISN_MEMBER/ISN_STRINGMEMBER

    ISN_PUT,	    // ":put", uses isn_arg.put
    ISN_IPUT,	    // ":iput", uses isn_arg.put

    ISN_CMDMOD,	    // set cmdmod
    ISN_CMDMOD_REV, // undo ISN_CMDMOD

    ISN_PROF_ಆರಂಭ, // start a line for profiling
    ISN_PROF_ಕೊನೆ,   // end a line for profiling

    ISN_DEBUG,	    // check for debug breakpoint, uses isn_arg.debug

    ISN_UNPACK,	    // unpack list into items, uses isn_arg.unpack
    ISN_SHUFFLE,    // move item on stack up or down
    ISN_DROP,	    // pop stack and discard value

    ISN_REDIRSTART, // :redir =>
    ISN_REDIREND,   // :redir END, isn_arg.number == 1 for append

    ISN_CEXPR_AUCMD, // first part of :cexpr  isn_arg.number is cmdidx
    ISN_CEXPR_CORE,  // second part of :cexpr, uses isn_arg.cexpr

    ISN_SCRIPTCTX_ಗಣ, // set script context for expression evaluation

    ISN_ಮುಕ್ತಾಯ	    // end marker in list of instructions
} isntype_T;


// arguments to ISN_BCALL
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    cbf_idx;	    // index in "global_functions"
    ಪೂರ್ಣಾಂಕ	    cbf_argcount;   // number of arguments on top of stack
} cbfunc_T;

// arguments to ISN_DCALL
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    cdf_idx;	    // index in "def_functions" for ISN_DCALL
    ಪೂರ್ಣಾಂಕ	    cdf_argcount;   // number of arguments on top of stack
} cdfunc_T;

// arguments to ISN_METHODCALL
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    class_T *cmf_itf;	    // interface used
    ಪೂರ್ಣಾಂಕ	    cmf_idx;	    // index in "def_functions" for ISN_METHODCALL
    ಪೂರ್ಣಾಂಕ	    cmf_argcount;   // number of arguments on top of stack
    ಪೂರ್ಣಾಂಕ	    cmf_is_super;   // doing "super.Func", use cmf_itf, not cmf_idx
    ufunc_T *cmf_ufunc;	    // for a generic method: the method of
			    // "cmf_itf" with the type arguments, NULL
			    // otherwise
    // Cache for a generic method: "cmf_cache_ufunc" is the method of
    // "cmf_cache_cl" with the type arguments of "cmf_ufunc", for the generic
    // method "cmf_cache_generic".  "cmf_cache_cl" is referenced so that the
    // functions are not freed.
    class_T *cmf_cache_cl;
    ufunc_T *cmf_cache_generic;
    ufunc_T *cmf_cache_ufunc;
} cmfunc_T;

// arguments to ISN_PCALL
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    cpf_ತುದಿ;	    // when TRUE partial is above the arguments
    ಪೂರ್ಣಾಂಕ	    cpf_argcount;   // number of arguments on top of stack
} cpfunc_T;

// arguments to ISN_UCALL and ISN_XCALL
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u  *cuf_name;
    ಪೂರ್ಣಾಂಕ	    cuf_argcount;   // number of arguments on top of stack
} cufunc_T;

// arguments to ISN_GETITEM
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    varnumber_T	gi_ಸೂಚ್ಯಂಕ;
    ಪೂರ್ಣಾಂಕ		gi_with_op;
} getitem_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    JUMP_ALWAYS,
    JUMP_NEVER,
    JUMP_IF_FALSE,		// pop and jump if false
    JUMP_WHILE_FALSE,		// pop and jump if false for :while
    JUMP_AND_KEEP_IF_TRUE,	// jump if top of stack is truthy, drop if not
    JUMP_IF_COND_TRUE,		// jump if top of stack is true, drop if not
    JUMP_IF_COND_FALSE,		// jump if top of stack is false, drop if not
} jumpwhen_T;

// arguments to ISN_JUMP
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    jumpwhen_T	jump_when;
    ಪೂರ್ಣಾಂಕ		jump_where;	// position to jump to
} jump_T;

// arguments to ISN_JUMP_IF_ARG_SET and ISN_JUMP_IF_ARG_NOT_SET
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		jump_arg_off;	// argument index, negative
    ಪೂರ್ಣಾಂಕ		jump_where;	// position to jump to
} jumparg_T;

// arguments to ISN_FOR
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಗಿಡ್ಡ	for_loop_idx;	// loop variable index
    ಪೂರ್ಣಾಂಕ		for_ಕೊನೆ;	// position to jump to after done
} forloop_T;

// arguments to ISN_WHILE
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಗಿಡ್ಡ	while_funcref_idx;  // variable index for funcref count
    ಪೂರ್ಣಾಂಕ		while_ಕೊನೆ;	    // position to jump to after done
} whileloop_T;

// arguments to ISN_ENDLOOP
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಗಿಡ್ಡ    ಕೊನೆ_funcref_idx;	// variable index of funcrefs.ga_len
    ಗಿಡ್ಡ    ಕೊನೆ_ಆಳ;		// nested loop depth
    ಗಿಡ್ಡ    ಕೊನೆ_var_idx;	// first variable declared in the loop
    ಗಿಡ್ಡ    ಕೊನೆ_var_ಎಣಿಕೆ;	// number of variables declared in the loop
} endloop_T;

// indirect arguments to ISN_TRY
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    try_catch;	    // position to jump to on throw
    ಪೂರ್ಣಾಂಕ	    try_finally;    // :finally or :endtry position to jump to
    ಪೂರ್ಣಾಂಕ	    try_endtry;	    // :endtry position to jump to
} tryref_T;

// arguments to ISN_TRY
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    tryref_T *try_ref;
} try_T;

// arguments to ISN_TRYCONT
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    tct_ಮಟ್ಟಗಳು;	    // number of nested try statements
    ಪೂರ್ಣಾಂಕ	    tct_where;	    // position to jump to, WHILE or FOR
} trycont_T;

// arguments to ISN_ECHO
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    echo_with_white;    // :echo instead of :echon
    ಪೂರ್ಣಾಂಕ	    echo_ಎಣಿಕೆ;		// number of expressions
} echo_T;

// arguments to ISN_OPNR, ISN_OPFLOAT, etc.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    exprtype_T	op_type;
    ಪೂರ್ಣಾಂಕ		op_ic;	    // TRUE with '#', FALSE with '?', else MAYBE
} opexpr_T;

// arguments to ISN_CHECKTYPE
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    type_T	*ct_type;
    int8_T	ct_off;		// offset in stack, -1 is bottom
    int8_T	ct_arg_idx;	// argument index or zero
    int8_T	ct_is_var;	// when TRUE checking variable instead of arg
} checktype_T;

// arguments to ISN_STORENR
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		stnr_idx;
    varnumber_T	stnr_val;
} storenr_T;

// arguments to ISN_STOREOPT and ISN_STOREFUNCOPT
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*so_name;
    ಪೂರ್ಣಾಂಕ		so_flags;
} storeopt_T;

// arguments to ISN_LOADS and ISN_STORES
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*ls_name;	// variable name (with s: for ISN_STORES)
    ಪೂರ್ಣಾಂಕ		ls_sid;		// script ID
} loadstore_T;

// arguments to ISN_LOADSCRIPT and ISN_STORESCRIPT
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		sref_sid;	// script ID
    ಪೂರ್ಣಾಂಕ		sref_idx;	// index in sn_var_vals
    ಪೂರ್ಣಾಂಕ		sref_seq;	// sn_script_seq when compiled
    type_T	*sref_type;	// type of the variable when compiled
} scriptref_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    scriptref_T	*scriptref;
} script_T;

// arguments to ISN_UNLET
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*ul_name;	// variable name with g:, w:, etc.
    ಪೂರ್ಣಾಂಕ		ul_forceit;	// forceit flag
} unlet_T;

// extra arguments for funcref_T
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	  *fre_func_name;	// function name for legacy function
    loopvarinfo_T fre_loopvar_info;	// info about variables inside loops
    class_T	  *fre_class;		// class for a method
    ಪೂರ್ಣಾಂಕ		  fre_object_ವಿಧಾನ;	// class or object method
    ಪೂರ್ಣಾಂಕ		  fre_ವಿಧಾನ_idx;	// method index on "fre_class"
    ಪೂರ್ಣಾಂಕ		  fre_outer_ಆಳ;	// when > 0: use the context of the
					// function this many levels up
} funcref_extra_T;

// arguments to ISN_FUNCREF
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		    fr_dfunc_idx;   // function index for :def function
    funcref_extra_T *fr_extra;	    // optional extra information
} funcref_T;

// arguments to ISN_NEWFUNC
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	  *nfa_lambda;	    // name of the lambda already defined
    char_u	  *nfa_global;	    // name of the global function to be created
    loopvarinfo_T nfa_loopvar_info; // ifno about variables inside loops
} newfuncarg_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    newfuncarg_T *nf_arg;
} newfunc_T;

// arguments to ISN_CHECKLEN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		cl_min_len;	// minimum length
    ಪೂರ್ಣಾಂಕ		cl_ಇನ್ನಷ್ಟು_OK;	// longer is allowed
} checklen_T;

// arguments to ISN_SHUFFLE
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		shfl_ಅಂಶ;	// item to move (relative to top of stack)
    ಪೂರ್ಣಾಂಕ		shfl_ಮೇಲೆ;	// places to move upwards
} shuffle_T;

// arguments to ISN_PUT
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		put_regname;	// register, can be NUL
    linenr_T	put_lnum;	// line number to put below
} put_T;

// arguments to ISN_CMDMOD
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    cmdmod_T	*cf_cmdmod;	// allocated
} cmod_T;

// arguments to ISN_UNPACK
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		unp_ಎಣಿಕೆ;	// number of items to produce
    ಪೂರ್ಣಾಂಕ		unp_semicolon;	// last item gets list of remainder
} unpack_T;

// arguments to ISN_LOADOUTER and ISN_STOREOUTER
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		outer_idx;	//ಸೂಚ್ಯಂಕ
    ಪೂರ್ಣಾಂಕ		outer_ಆಳ;	// nesting level, stack frames to go up;
				// negative for a loop variable, the loop
				// depth is -outer_depth - 1
} isn_outer_T;

// arguments to ISN_SUBSTITUTE
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*subs_cmd;	// :s command
    isn_T	*subs_instr;	// sequence of instructions
} subs_T;

// indirect arguments to ISN_TRY
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		cer_cmdidx;
    char_u	*cer_cmdline;
    ಪೂರ್ಣಾಂಕ		cer_forceit;
} cexprref_T;

// arguments to ISN_CEXPR_CORE
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    cexprref_T *cexpr_ref;
} cexpr_T;

// arguments to ISN_2STRING and ISN_2STRING_ANY
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ		flags;
} tostring_T;

// arguments to ISN_2BOOL
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ		invert;
} tobool_T;

// arguments to ISN_DEBUG
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    varnumber_T	dbg_var_ಹೆಸರುಗಳು_len;  // current number of local variables
    ಪೂರ್ಣಾಂಕ		dbg_break_lnum;	    // first line to break after
} debug_T;

// arguments to ISN_DEFER
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		defer_var_idx;	    // local variable index for defer list
    ಪೂರ್ಣಾಂಕ		defer_argcount;	    // number of arguments
} deferins_T;

// arguments to ISN_ECHOWINDOW
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		ewin_ಎಣಿಕೆ;	    // number of arguments
    ದೀರ್ಘ	ewin_ಸಮಯ;	    // time argument (msec)
} echowin_T;

// arguments to ISN_CONSTRUCT
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		construct_ಗಾತ್ರ;	    // size of object in bytes
    class_T	*construct_class;   // class the object is created from
} construct_T;

// arguments to ISN_STORE_CLASSMEMBER, ISN_LOAD_CLASSMEMBER, ISN_GET_ITF_MEMBER
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    class_T	*cm_class;
    ಪೂರ್ಣಾಂಕ		cm_idx;
} classmember_T;

// arguments to ISN_STOREINDEX
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    vartype_T	si_vartype;
    class_T	*si_class;
} storeindex_T;

// arguments to ISN_LOCKUNLOCK
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*lu_string;	// for exec_command
    class_T	*lu_cl_exec;	// executing, null if not class/obj method
    ಪೂರ್ಣಾಂಕ		lu_is_arg;	// is lval_root a function arg
} lockunlock_T;

/*
 * Instruction
 */
ರಚನೆ isn_S {
    isntype_T	isn_type;
    ಪೂರ್ಣಾಂಕ		isn_lnum;
    ಒಕ್ಕೂಟ {
	char_u		    *string;
	varnumber_T	    ಸಂಖ್ಯೆ;
	blob_T		    *blob;
	vartype_T	    vartype;
	float_T		    fnumber;
	channel_T	    *channel;
	ಕೆಲಸ_T		    *ಕೆಲಸ;
	partial_T	    *partial;
	class_T		    *classarg;
	jump_T		    jump;
	jumparg_T	    jumparg;
	forloop_T	    forloop;
	whileloop_T	    whileloop;
	endloop_T	    endloop;
	try_T		    tryref;
	trycont_T	    trycont;
	cbfunc_T	    bfunc;
	cdfunc_T	    dfunc;
	cmfunc_T	    *mfunc;
	cpfunc_T	    pfunc;
	cufunc_T	    ufunc;
	echo_T		    echo;
	opexpr_T	    op;
	checktype_T	    type;
	storenr_T	    storenr;
	storeopt_T	    storeopt;
	loadstore_T	    loadstore;
	script_T	    script;
	unlet_T		    unlet;
	funcref_T	    funcref;
	newfunc_T	    newfunc;
	checklen_T	    checklen;
	shuffle_T	    shuffle;
	put_T		    put;
	cmod_T		    cmdmod;
	unpack_T	    unpack;
	isn_outer_T	    outer;
	subs_T		    subs;
	cexpr_T		    cexpr;
	isn_T		    *instr;
	tostring_T	    tostring;
	tobool_T	    tobool;
	getitem_T	    getitem;
	debug_T		    debug;
	deferins_T	    defer;
	echowin_T	    echowin;
	construct_T	    construct;
	classmember_T	    classmember;
	storeindex_T	    storeindex;
	lockunlock_T	    lockunlock;
	sctx_T		    setsctx;
    } isn_arg;
};

/*
 * Info about a function defined with :def.  Used in "def_functions".
 */
ರಚನೆ dfunc_S {
    ufunc_T	*df_ufunc;	    // struct containing most stuff
    ಪೂರ್ಣಾಂಕ		df_refcount;	    // how many ufunc_T point to this dfunc_T
    ಪೂರ್ಣಾಂಕ		df_idx;		    // index in def_functions
    ಅಕ್ಷರ	df_deleted;	    // if TRUE function was deleted
    ಅಕ್ಷರ	df_delete_busy;	    // TRUE when in
				    // delete_def_function_contents()
    ಪೂರ್ಣಾಂಕ		df_script_seq;	    // Value of sctx_T sc_seq when the function
				    // was compiled.
    char_u	*df_name;	    // name used for error messages

    garray_T	df_def_ವಾದಗಳು_isn;    // default argument instructions
    garray_T	df_var_ಹೆಸರುಗಳು;	    // names of local vars

    // After compiling "df_instr" and/or "df_instr_prof" is not NULL.
    isn_T	*df_instr;	    // function body to be executed
    ಪೂರ್ಣಾಂಕ		df_instr_ಎಣಿಕೆ;	    // size of "df_instr"
    ಪೂರ್ಣಾಂಕ		df_instr_debug_ಎಣಿಕೆ; // size of "df_instr_debug"
    isn_T	*df_instr_debug;      // like "df_instr" with debugging
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    isn_T	*df_instr_prof;	     // like "df_instr" with profiling
    ಪೂರ್ಣಾಂಕ		df_instr_prof_ಎಣಿಕೆ; // size of "df_instr_prof"
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ		df_varcount;	    // number of local variables
    ಪೂರ್ಣಾಂಕ		df_has_ಆವರಣ;	    // one if a closure was created
    ಪೂರ್ಣಾಂಕ		df_defer_var_idx;   // index of local variable that has a list
				    // of deferred function calls; zero if not
				    //ಗಣ
};

// Number of entries used by stack frame for a function call.
// - ec_dfunc_idx:   function index
// - ec_iidx:	     instruction index
// - ec_instr:       instruction list pointer
// - ec_outer:	     stack used for closures
// - funclocal:	     function-local data
// - ec_frame_idx:   previous frame index
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಚೌಕಟ್ಟು_FUNC_OFF 0
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಚೌಕಟ್ಟು_IIDX_OFF 1
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಚೌಕಟ್ಟು_INSTR_OFF 2
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಚೌಕಟ್ಟು_OUTER_OFF 3
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಚೌಕಟ್ಟು_FUNCLOCAL_OFF 4
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಚೌಕಟ್ಟು_IDX_OFF 5
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ 6


ಬಾಹ್ಯ garray_T def_functions;

// Used for "lnum" when a range is to be taken from the stack.
#ವ್ಯಾಖ್ಯಾನಿಸು LNUM_VARIABLE_RANGE (-999)

// Used for "lnum" when a range is to be taken from the stack and "!" is used.
#ವ್ಯಾಖ್ಯಾನಿಸು LNUM_VARIABLE_RANGE_ABOVE (-888)

// Keep in sync with get_compile_type()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
# define INSTRUCTIONS(dfunc) \
	(debug_break_level > 0 || may_break_ಒಳ_function(dfunc->df_ufunc) \
	    ? (dfunc)->df_instr_debug \
	    : ((do_profiling == PROF_YES && (dfunc->df_ufunc)->uf_profiling) \
		? (dfunc)->df_instr_prof \
		: (dfunc)->df_instr))
#ಇಲ್ಲದಿದ್ದರೆ
# define INSTRUCTIONS(dfunc) \
	(debug_break_level > 0 || may_break_ಒಳ_function((dfunc)->df_ufunc) \
		? (dfunc)->df_instr_debug \
		: (dfunc)->df_instr)
#ಆದರೆ_ಅಂತ್ಯ

// Structure passed between the compile_expr* functions to keep track of
// constants that have been parsed but for which no code was produced yet.  If
// possible expressions on these constants are applied at compile time.  If
// that is not possible, the code to push the constants needs to be generated
// before other instructions.
// Using 50 should be more than enough of 5 levels of ().
#ವ್ಯಾಖ್ಯಾನಿಸು PPSIZE 50
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    typval_T	pp_tv[PPSIZE];	// stack of ppconst constants
    ಪೂರ್ಣಾಂಕ		pp_ಬಳಸಿದ;	// active entries in pp_tv[]
    ಪೂರ್ಣಾಂಕ		pp_is_const;	// all generated code was constants, used for a
				// list or dict with constant members
} ppconst_T;

// values for ctx_skip
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    SKIP_NOT,		// condition is a constant, produce code
    SKIP_YES,		// condition is a constant, do NOT produce code
    SKIP_UNKNOWN	// condition is not a constant, produce code
} skip_T;

/*
 * Chain of jump instructions where the end label needs to be set.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ endlabel_S endlabel_T;
ರಚನೆ endlabel_S {
    endlabel_T	*el_ಮುಂದಿನ;	    // chain end_label locations
    ಪೂರ್ಣಾಂಕ		el_ಕೊನೆ_label;	    // instruction idx where to set end
};

/*
 * info specific for the scope of :if / elseif / else
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		is_ನೋಡಿದ_else;
    ಪೂರ್ಣಾಂಕ		is_ನೋಡಿದ_skip_not;   // a block was unconditionally executed
    ಪೂರ್ಣಾಂಕ		is_had_return;	    // every block ends in :return
    ಪೂರ್ಣಾಂಕ		is_if_label;	    // instruction idx at IF or ELSEIF
    endlabel_T	*is_ಕೊನೆ_label;	    // instructions to set end label
} ifscope_T;

// info used by :for and :while needed for ENDLOOP
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    li_local_ಎಣಿಕೆ;	    // ctx_locals.ga_len at loop start
    ಪೂರ್ಣಾಂಕ	    li_ಆವರಣ_ಎಣಿಕೆ;	    // ctx_closure_count at loop start
    ಪೂರ್ಣಾಂಕ	    li_funcref_idx;	    // index of var that holds funcref count
    ಪೂರ್ಣಾಂಕ	    li_ಆಳ;		    // nested loop depth
} loop_info_T;

/*
 * info specific for the scope of :while
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		ws_ತುದಿ_label;	    // instruction idx at WHILE
    endlabel_T	*ws_ಕೊನೆ_label;	    // instructions to set end
    loop_info_T ws_loop_info;	    // info for LOOPEND
} whilescope_T;

/*
 * info specific for the scope of :for
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		fs_ತುದಿ_label;	    // instruction idx at FOR
    endlabel_T	*fs_ಕೊನೆ_label;	    // break instructions
    loop_info_T	fs_loop_info;	    // info for LOOPEND
} forscope_T;

/*
 * info specific for the scope of :try
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		ts_try_label;	    // instruction idx at TRY
    endlabel_T	*ts_ಕೊನೆ_label;	    // jump to :finally or :endtry
    ಪೂರ್ಣಾಂಕ		ts_catch_label;	    // instruction idx of last CATCH
    ಪೂರ್ಣಾಂಕ		ts_caught_all;	    // "catch" without argument encountered
    ಪೂರ್ಣಾಂಕ		ts_has_finally;	    // "finally" encountered
    ಪೂರ್ಣಾಂಕ		ts_no_return;	    // one of the blocks did not end in return
} tryscope_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    NO_SCOPE,
    IF_SCOPE,
    WHILE_SCOPE,
    FOR_SCOPE,
    TRY_SCOPE,
    ಖಂಡ_SCOPE
} scopetype_T;

/*
 * Info for one scope, pointed to by "ctx_scope".
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ scope_S scope_T;
ರಚನೆ scope_S {
    scope_T	*se_outer;	    // scope containing this one
    scopetype_T se_type;
    ಪೂರ್ಣಾಂಕ		se_local_ಎಣಿಕೆ;	    // ctx_locals.ga_len before scope
    skip_T	se_skip_save;	    // ctx_skip before the block
    ಪೂರ್ಣಾಂಕ		se_loop_ಆಳ;	    // number of loop scopes, including this
    ಒಕ್ಕೂಟ {
	ifscope_T	se_if;
	whilescope_T	se_while;
	forscope_T	se_for;
	tryscope_T	se_try;
    } se_u;
};

/*
 * Entry for "ctx_locals".  Used for arguments and local variables.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*lv_name;
    type_T	*lv_type;
    ಪೂರ್ಣಾಂಕ		lv_idx;		// index of the variable on the stack
    ಪೂರ್ಣಾಂಕ		lv_loop_ಆಳ;	// depth for variable inside a loop or -1
    ಪೂರ್ಣಾಂಕ		lv_loop_idx;	// index of first variable inside a loop or -1
    ಪೂರ್ಣಾಂಕ		lv_from_outer;	// nesting level, using ctx_outer scope
    ಪೂರ್ಣಾಂಕ		lv_const;	// ASSIGN_VAR (can be assigned to),
				// ASSIGN_FINAL (no assignment) or ASSIGN_CONST
				// (value cannot be changed)
    ಪೂರ್ಣಾಂಕ		lv_arg;		// when TRUE this is an argument
    ufunc_T	*lv_ufunc;	// for a nested function: its ufunc, see
				// compile_nested_generic_func()
} lvar_T;

// Destination for an assignment or ":unlet" with an index.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    dest_local,
    dest_option,
    dest_func_option,
    dest_env,
    dest_global,
    dest_ಬಫರ್,
    dest_ಕಿಟಕಿ,
    dest_tab,
    dest_vimvar,
    dest_class_member,
    dest_script,
    dest_script_v9,
    dest_reg,
    dest_ಅಭಿವ್ಯಕ್ತಿ,
} ನಿಯೋಜಿಸು_dest_T;

// Used by compile_lhs() to store information about the LHS of an assignment
// and one argument of ":unlet" with an index.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ನಿಯೋಜಿಸು_dest_T   lhs_dest;	    // type of destination

    char_u	    *lhs_name;	    // allocated name excluding the last
				    // "[expr]" or ".name".
    size_t	    lhs_varlen;	    // length of the variable without
				    // "[expr]" or ".name"
    char_u	    *lhs_ಸಂಪೂರ್ಣ;	    // allocated name including the last
				    // "[expr]" or ".name" for :redir
    size_t	    lhs_varlen_ಒಟ್ಟು; // length of the variable including
				      // any "[expr]" or ".name"
    char_u	    *lhs_dest_ಕೊನೆ;  // end of the destination, including
				    // "[expr]" or ".name".
    char_u	    *lhs_ಕೊನೆ;	    // end including any type

    ಪೂರ್ಣಾಂಕ		    lhs_has_ಸೂಚ್ಯಂಕ;  // has "[expr]" or ".name"

    ಪೂರ್ಣಾಂಕ		    lhs_new_local;  // create new local variable
    ಪೂರ್ಣಾಂಕ		    lhs_opt_flags;  // for when destination is an option
    ಪೂರ್ಣಾಂಕ		    lhs_vimvaridx;  // for when destination is a v:var

    lvar_T	    lhs_local_lvar; // used for existing local destination
    lvar_T	    lhs_arg_lvar;   // used for argument destination
    lvar_T	    *lhs_lvar;	    // points to destination lvar

    class_T	    *lhs_class;		    // for dest_class_member
    ಪೂರ್ಣಾಂಕ		    lhs_classmember_idx;    // for dest_class_member

    ಪೂರ್ಣಾಂಕ		    lhs_scriptvar_sid;
    ಪೂರ್ಣಾಂಕ		    lhs_scriptvar_idx;

    ಪೂರ್ಣಾಂಕ		    lhs_has_type;   // type was specified
    type_T	    *lhs_type;
    ಪೂರ್ಣಾಂಕ		    lhs_member_idx;    // object member index
    type_T	    *lhs_member_type;  // list/dict/object member type

    ಪೂರ್ಣಾಂಕ		    lhs_append;	    // used by ISN_REDIREND
} lhs_T;

/*
 * Context for compiling lines of a :def function.
 * Stores info about the local variables and condition stack.
 */
ರಚನೆ cctx_S {
    ufunc_T	*ctx_ufunc;	    // current function
    ಪೂರ್ಣಾಂಕ		ctx_lnum;	    // line number in current function
    char_u	*ctx_ಸಾಲು_ಆರಂಭ;    // start of current line or NULL
    garray_T	ctx_instr;	    // generated instructions

    ಪೂರ್ಣಾಂಕ		ctx_prev_lnum;	    // line number below previous command, for
				    // debugging

    compiletype_T ctx_compile_type;

    garray_T	ctx_locals;	    // currently visible local variables

    ಪೂರ್ಣಾಂಕ		ctx_has_ಆವರಣ;    // set to one if a FUNCREF was used in the
				    // function
    ಪೂರ್ಣಾಂಕ		ctx_ಆವರಣ_ಎಣಿಕೆ;  // incremented for each closure created in
				    // the function.

    skip_T	ctx_skip;
    scope_T	*ctx_scope;	    // current scope, NULL at toplevel
    ಪೂರ್ಣಾಂಕ		ctx_had_return;	    // last seen statement was "return"
    ಪೂರ್ಣಾಂಕ		ctx_had_throw;	    // last seen statement was "throw"

    cctx_T	*ctx_outer;	    // outer scope for lambda or nested
				    // function
    ಪೂರ್ಣಾಂಕ		ctx_outer_ಬಳಸಿದ;	    // var in ctx_outer was used

    garray_T	ctx_type_ರಾಶಿ;	    // type of each item on the stack
    garray_T	*ctx_type_list;	    // list of pointers to allocated types

    ಪೂರ್ಣಾಂಕ		ctx_has_cmdmod;	    // ISN_CMDMOD was generated

    lhs_T	ctx_redir_lhs;	    // LHS for ":redir => var", valid when
				    // lhs_name is not NULL
};

/*
 * List of special functions for "compile_arguments()".
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    CA_NOT_SPECIAL,
    CA_SEARCHPAIR,	    // {skip} in searchpair() and searchpairpos()
    CA_SUBSTITUTE,	    // {sub} in substitute(), when prefixed with \=
} ca_special_T;

// flags for typval2type()
#ವ್ಯಾಖ್ಯಾನಿಸು TVTT_DO_MEMBER	    1
#ವ್ಯಾಖ್ಯಾನಿಸು TVTT_ಇನ್ನಷ್ಟು_SPECIFIC  2	// get most specific type for member

// flags for call_def_function()
#ವ್ಯಾಖ್ಯಾನಿಸು DEF_USE_PT_ARGV	    1	// use the partial arguments

// Flag used for conversion to string by may_generate_2STRING()
#ವ್ಯಾಖ್ಯಾನಿಸು TOSTRING_NONE		0x0
// Convert a List to series of values separated by newline
#ವ್ಯಾಖ್ಯಾನಿಸು TOSTRING_INTERPOLATE	0x1
// Convert a List to a textual representation of the list "[...]"
#ವ್ಯಾಖ್ಯಾನಿಸು TOSTRING_TOLERANT	0x2
