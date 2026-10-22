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

#اگر_تعریف_شدہ VMS
# include <float.h>
#اگر_ختم

قسمتعریف شمار {
    ISN_EXEC,	    // execute Ex command line isn_arg.string
    ISN_EXECCONCAT, // execute Ex command from isn_arg.number items on stack
    ISN_EXEC_بانٹو, // execute Ex command from isn_arg.string split at NL
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

    ISN_ماخذ,	    // source autoload script, isn_arg.number is the script ID
    ISN_INSTR,	    // instructions compiled from expression
    ISN_CONSTRUCT,  // construct an object, using construct_T
    ISN_GET_OBJ_MEMBER, // object member, index is isn_arg.number
    ISN_GET_ITF_MEMBER, // interface member, index is isn_arg.classmember
    ISN_STORE_THIS, // store value in "this" object member, index is
		    // isn_arg.number
    ISN_بھرو_CLASSMEMBER,  // load class member, using isn_arg.classmember
    ISN_STORE_CLASSMEMBER,  // store in class member, using isn_arg.classmember

    // get and set variables
    ISN_بھرو,	    // push local variable isn_arg.number
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
    ISN_PCALL_انتہا,  // cleanup after ISN_PCALL with cpf_top set
    ISN_RETURN,	    // return, result is on top of stack
    ISN_RETURN_VOID, // Push void, then return
    ISN_RETURN_OBJECT, // Push constructed object, then return
    ISN_FUNCREF,    // push a function ref to dfunc isn_arg.funcref
    ISN_NEWFUNC,    // create a global function from a lambda function
    ISN_DEF,	    // list functions
    ISN_DEFER,	    // :defer  argument count is isn_arg.number

    // expression operations
    ISN_JUMP,	    // jump if condition is matched isn_arg.jump
    ISN_JUMP_IF_ARG_سیٹ, // jump if argument is already set, uses
			 // isn_arg.jumparg
    ISN_JUMP_IF_ARG_NOT_سیٹ, // jump if argument is not set, uses
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
    ISN_قاش,	    // drop isn_arg.number items from start of list
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

    ISN_PROF_آغاز, // start a line for profiling
    ISN_PROF_انتہا,   // end a line for profiling

    ISN_DEBUG,	    // check for debug breakpoint, uses isn_arg.debug

    ISN_UNPACK,	    // unpack list into items, uses isn_arg.unpack
    ISN_SHUFFLE,    // move item on stack up or down
    ISN_DROP,	    // pop stack and discard value

    ISN_REDIRSTART, // :redir =>
    ISN_REDIREND,   // :redir END, isn_arg.number == 1 for append

    ISN_CEXPR_AUCMD, // first part of :cexpr  isn_arg.number is cmdidx
    ISN_CEXPR_CORE,  // second part of :cexpr, uses isn_arg.cexpr

    ISN_SCRIPTCTX_سیٹ, // set script context for expression evaluation

    ISN_اختتام	    // end marker in list of instructions
} isntype_T;


// arguments to ISN_BCALL
قسمتعریف ساخت {
    صحیح	    cbf_idx;	    // index in "global_functions"
    صحیح	    cbf_argcount;   // number of arguments on top of stack
} cbfunc_T;

// arguments to ISN_DCALL
قسمتعریف ساخت {
    صحیح	    cdf_idx;	    // index in "def_functions" for ISN_DCALL
    صحیح	    cdf_argcount;   // number of arguments on top of stack
} cdfunc_T;

// arguments to ISN_METHODCALL
قسمتعریف ساخت {
    class_T *cmf_itf;	    // interface used
    صحیح	    cmf_idx;	    // index in "def_functions" for ISN_METHODCALL
    صحیح	    cmf_argcount;   // number of arguments on top of stack
    صحیح	    cmf_is_super;   // doing "super.Func", use cmf_itf, not cmf_idx
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
قسمتعریف ساخت {
    صحیح	    cpf_چوٹی;	    // when TRUE partial is above the arguments
    صحیح	    cpf_argcount;   // number of arguments on top of stack
} cpfunc_T;

// arguments to ISN_UCALL and ISN_XCALL
قسمتعریف ساخت {
    char_u  *cuf_name;
    صحیح	    cuf_argcount;   // number of arguments on top of stack
} cufunc_T;

// arguments to ISN_GETITEM
قسمتعریف ساخت {
    varnumber_T	gi_اشاریہ;
    صحیح		gi_with_op;
} getitem_T;

قسمتعریف شمار {
    JUMP_ALWAYS,
    JUMP_NEVER,
    JUMP_IF_FALSE,		// pop and jump if false
    JUMP_WHILE_FALSE,		// pop and jump if false for :while
    JUMP_AND_KEEP_IF_TRUE,	// jump if top of stack is truthy, drop if not
    JUMP_IF_COND_TRUE,		// jump if top of stack is true, drop if not
    JUMP_IF_COND_FALSE,		// jump if top of stack is false, drop if not
} jumpwhen_T;

// arguments to ISN_JUMP
قسمتعریف ساخت {
    jumpwhen_T	jump_when;
    صحیح		jump_where;	// position to jump to
} jump_T;

// arguments to ISN_JUMP_IF_ARG_SET and ISN_JUMP_IF_ARG_NOT_SET
قسمتعریف ساخت {
    صحیح		jump_arg_off;	// argument index, negative
    صحیح		jump_where;	// position to jump to
} jumparg_T;

// arguments to ISN_FOR
قسمتعریف ساخت {
    مختصر	for_loop_idx;	// loop variable index
    صحیح		for_انتہا;	// position to jump to after done
} forloop_T;

// arguments to ISN_WHILE
قسمتعریف ساخت {
    مختصر	while_funcref_idx;  // variable index for funcref count
    صحیح		while_انتہا;	    // position to jump to after done
} whileloop_T;

// arguments to ISN_ENDLOOP
قسمتعریف ساخت {
    مختصر    انتہا_funcref_idx;	// variable index of funcrefs.ga_len
    مختصر    انتہا_گہرائی;		// nested loop depth
    مختصر    انتہا_var_idx;	// first variable declared in the loop
    مختصر    انتہا_var_گنتی;	// number of variables declared in the loop
} endloop_T;

// indirect arguments to ISN_TRY
قسمتعریف ساخت {
    صحیح	    try_catch;	    // position to jump to on throw
    صحیح	    try_finally;    // :finally or :endtry position to jump to
    صحیح	    try_endtry;	    // :endtry position to jump to
} tryref_T;

// arguments to ISN_TRY
قسمتعریف ساخت {
    tryref_T *try_ref;
} try_T;

// arguments to ISN_TRYCONT
قسمتعریف ساخت {
    صحیح	    tct_سطحیں;	    // number of nested try statements
    صحیح	    tct_where;	    // position to jump to, WHILE or FOR
} trycont_T;

// arguments to ISN_ECHO
قسمتعریف ساخت {
    صحیح	    echo_with_white;    // :echo instead of :echon
    صحیح	    echo_گنتی;		// number of expressions
} echo_T;

// arguments to ISN_OPNR, ISN_OPFLOAT, etc.
قسمتعریف ساخت {
    exprtype_T	op_type;
    صحیح		op_ic;	    // TRUE with '#', FALSE with '?', else MAYBE
} opexpr_T;

// arguments to ISN_CHECKTYPE
قسمتعریف ساخت {
    type_T	*ct_type;
    int8_T	ct_off;		// offset in stack, -1 is bottom
    int8_T	ct_arg_idx;	// argument index or zero
    int8_T	ct_is_var;	// when TRUE checking variable instead of arg
} checktype_T;

// arguments to ISN_STORENR
قسمتعریف ساخت {
    صحیح		stnr_idx;
    varnumber_T	stnr_val;
} storenr_T;

// arguments to ISN_STOREOPT and ISN_STOREFUNCOPT
قسمتعریف ساخت {
    char_u	*so_name;
    صحیح		so_flags;
} storeopt_T;

// arguments to ISN_LOADS and ISN_STORES
قسمتعریف ساخت {
    char_u	*ls_name;	// variable name (with s: for ISN_STORES)
    صحیح		ls_sid;		// script ID
} loadstore_T;

// arguments to ISN_LOADSCRIPT and ISN_STORESCRIPT
قسمتعریف ساخت {
    صحیح		sref_sid;	// script ID
    صحیح		sref_idx;	// index in sn_var_vals
    صحیح		sref_seq;	// sn_script_seq when compiled
    type_T	*sref_type;	// type of the variable when compiled
} scriptref_T;

قسمتعریف ساخت {
    scriptref_T	*scriptref;
} script_T;

// arguments to ISN_UNLET
قسمتعریف ساخت {
    char_u	*ul_name;	// variable name with g:, w:, etc.
    صحیح		ul_forceit;	// forceit flag
} unlet_T;

// extra arguments for funcref_T
قسمتعریف ساخت {
    char_u	  *fre_func_name;	// function name for legacy function
    loopvarinfo_T fre_loopvar_info;	// info about variables inside loops
    class_T	  *fre_class;		// class for a method
    صحیح		  fre_object_طریقہ;	// class or object method
    صحیح		  fre_طریقہ_idx;	// method index on "fre_class"
    صحیح		  fre_outer_گہرائی;	// when > 0: use the context of the
					// function this many levels up
} funcref_extra_T;

// arguments to ISN_FUNCREF
قسمتعریف ساخت {
    صحیح		    fr_dfunc_idx;   // function index for :def function
    funcref_extra_T *fr_extra;	    // optional extra information
} funcref_T;

// arguments to ISN_NEWFUNC
قسمتعریف ساخت {
    char_u	  *nfa_lambda;	    // name of the lambda already defined
    char_u	  *nfa_global;	    // name of the global function to be created
    loopvarinfo_T nfa_loopvar_info; // ifno about variables inside loops
} newfuncarg_T;

قسمتعریف ساخت {
    newfuncarg_T *nf_arg;
} newfunc_T;

// arguments to ISN_CHECKLEN
قسمتعریف ساخت {
    صحیح		cl_min_len;	// minimum length
    صحیح		cl_مزید_OK;	// longer is allowed
} checklen_T;

// arguments to ISN_SHUFFLE
قسمتعریف ساخت {
    صحیح		shfl_شے;	// item to move (relative to top of stack)
    صحیح		shfl_اوپر;	// places to move upwards
} shuffle_T;

// arguments to ISN_PUT
قسمتعریف ساخت {
    صحیح		put_regname;	// register, can be NUL
    linenr_T	put_lnum;	// line number to put below
} put_T;

// arguments to ISN_CMDMOD
قسمتعریف ساخت {
    cmdmod_T	*cf_cmdmod;	// allocated
} cmod_T;

// arguments to ISN_UNPACK
قسمتعریف ساخت {
    صحیح		unp_گنتی;	// number of items to produce
    صحیح		unp_semicolon;	// last item gets list of remainder
} unpack_T;

// arguments to ISN_LOADOUTER and ISN_STOREOUTER
قسمتعریف ساخت {
    صحیح		outer_idx;	//اشاریہ
    صحیح		outer_گہرائی;	// nesting level, stack frames to go up;
				// negative for a loop variable, the loop
				// depth is -outer_depth - 1
} isn_outer_T;

// arguments to ISN_SUBSTITUTE
قسمتعریف ساخت {
    char_u	*subs_cmd;	// :s command
    isn_T	*subs_instr;	// sequence of instructions
} subs_T;

// indirect arguments to ISN_TRY
قسمتعریف ساخت {
    صحیح		cer_cmdidx;
    char_u	*cer_cmdline;
    صحیح		cer_forceit;
} cexprref_T;

// arguments to ISN_CEXPR_CORE
قسمتعریف ساخت {
    cexprref_T *cexpr_ref;
} cexpr_T;

// arguments to ISN_2STRING and ISN_2STRING_ANY
قسمتعریف ساخت {
    صحیح		انحراف;
    صحیح		flags;
} tostring_T;

// arguments to ISN_2BOOL
قسمتعریف ساخت {
    صحیح		انحراف;
    صحیح		invert;
} tobool_T;

// arguments to ISN_DEBUG
قسمتعریف ساخت {
    varnumber_T	dbg_var_نام_len;  // current number of local variables
    صحیح		dbg_break_lnum;	    // first line to break after
} debug_T;

// arguments to ISN_DEFER
قسمتعریف ساخت {
    صحیح		defer_var_idx;	    // local variable index for defer list
    صحیح		defer_argcount;	    // number of arguments
} deferins_T;

// arguments to ISN_ECHOWINDOW
قسمتعریف ساخت {
    صحیح		ewin_گنتی;	    // number of arguments
    طویل	ewin_وقت;	    // time argument (msec)
} echowin_T;

// arguments to ISN_CONSTRUCT
قسمتعریف ساخت {
    صحیح		construct_سائز;	    // size of object in bytes
    class_T	*construct_class;   // class the object is created from
} construct_T;

// arguments to ISN_STORE_CLASSMEMBER, ISN_LOAD_CLASSMEMBER, ISN_GET_ITF_MEMBER
قسمتعریف ساخت {
    class_T	*cm_class;
    صحیح		cm_idx;
} classmember_T;

// arguments to ISN_STOREINDEX
قسمتعریف ساخت {
    vartype_T	si_vartype;
    class_T	*si_class;
} storeindex_T;

// arguments to ISN_LOCKUNLOCK
قسمتعریف ساخت {
    char_u	*lu_string;	// for exec_command
    class_T	*lu_cl_exec;	// executing, null if not class/obj method
    صحیح		lu_is_arg;	// is lval_root a function arg
} lockunlock_T;

/*
 * Instruction
 */
ساخت isn_S {
    isntype_T	isn_type;
    صحیح		isn_lnum;
    اتحاد {
	char_u		    *string;
	varnumber_T	    نمبر;
	blob_T		    *blob;
	vartype_T	    vartype;
	float_T		    fnumber;
	channel_T	    *channel;
	کام_T		    *کام;
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
ساخت dfunc_S {
    ufunc_T	*df_ufunc;	    // struct containing most stuff
    صحیح		df_refcount;	    // how many ufunc_T point to this dfunc_T
    صحیح		df_idx;		    // index in def_functions
    حرف	df_deleted;	    // if TRUE function was deleted
    حرف	df_delete_busy;	    // TRUE when in
				    // delete_def_function_contents()
    صحیح		df_script_seq;	    // Value of sctx_T sc_seq when the function
				    // was compiled.
    char_u	*df_name;	    // name used for error messages

    garray_T	df_def_دلائل_isn;    // default argument instructions
    garray_T	df_var_نام;	    // names of local vars

    // After compiling "df_instr" and/or "df_instr_prof" is not NULL.
    isn_T	*df_instr;	    // function body to be executed
    صحیح		df_instr_گنتی;	    // size of "df_instr"
    صحیح		df_instr_debug_گنتی; // size of "df_instr_debug"
    isn_T	*df_instr_debug;      // like "df_instr" with debugging
#اگر_تعریف_شدہ FEAT_PROFILE
    isn_T	*df_instr_prof;	     // like "df_instr" with profiling
    صحیح		df_instr_prof_گنتی; // size of "df_instr_prof"
#اگر_ختم

    صحیح		df_varcount;	    // number of local variables
    صحیح		df_has_احاطہ;	    // one if a closure was created
    صحیح		df_defer_var_idx;   // index of local variable that has a list
				    // of deferred function calls; zero if not
				    //سیٹ
};

// Number of entries used by stack frame for a function call.
// - ec_dfunc_idx:   function index
// - ec_iidx:	     instruction index
// - ec_instr:       instruction list pointer
// - ec_outer:	     stack used for closures
// - funclocal:	     function-local data
// - ec_frame_idx:   previous frame index
#تعریف_کرو ڈھیر_چوکھٹا_FUNC_OFF 0
#تعریف_کرو ڈھیر_چوکھٹا_IIDX_OFF 1
#تعریف_کرو ڈھیر_چوکھٹا_INSTR_OFF 2
#تعریف_کرو ڈھیر_چوکھٹا_OUTER_OFF 3
#تعریف_کرو ڈھیر_چوکھٹا_FUNCLOCAL_OFF 4
#تعریف_کرو ڈھیر_چوکھٹا_IDX_OFF 5
#تعریف_کرو ڈھیر_چوکھٹا_سائز 6


خارجی garray_T def_functions;

// Used for "lnum" when a range is to be taken from the stack.
#تعریف_کرو LNUM_VARIABLE_RANGE (-999)

// Used for "lnum" when a range is to be taken from the stack and "!" is used.
#تعریف_کرو LNUM_VARIABLE_RANGE_ABOVE (-888)

// Keep in sync with get_compile_type()
#اگر_تعریف_شدہ FEAT_PROFILE
# define INSTRUCTIONS(dfunc) \
	(debug_break_level > 0 || may_break_اندر_function(dfunc->df_ufunc) \
	    ? (dfunc)->df_instr_debug \
	    : ((do_profiling == PROF_YES && (dfunc->df_ufunc)->uf_profiling) \
		? (dfunc)->df_instr_prof \
		: (dfunc)->df_instr))
#ورنہ
# define INSTRUCTIONS(dfunc) \
	(debug_break_level > 0 || may_break_اندر_function((dfunc)->df_ufunc) \
		? (dfunc)->df_instr_debug \
		: (dfunc)->df_instr)
#اگر_ختم

// Structure passed between the compile_expr* functions to keep track of
// constants that have been parsed but for which no code was produced yet.  If
// possible expressions on these constants are applied at compile time.  If
// that is not possible, the code to push the constants needs to be generated
// before other instructions.
// Using 50 should be more than enough of 5 levels of ().
#تعریف_کرو PPSIZE 50
قسمتعریف ساخت {
    typval_T	pp_tv[PPSIZE];	// stack of ppconst constants
    صحیح		pp_مستعمل;	// active entries in pp_tv[]
    صحیح		pp_is_const;	// all generated code was constants, used for a
				// list or dict with constant members
} ppconst_T;

// values for ctx_skip
قسمتعریف شمار {
    SKIP_NOT,		// condition is a constant, produce code
    SKIP_YES,		// condition is a constant, do NOT produce code
    SKIP_UNKNOWN	// condition is not a constant, produce code
} skip_T;

/*
 * Chain of jump instructions where the end label needs to be set.
 */
قسمتعریف ساخت endlabel_S endlabel_T;
ساخت endlabel_S {
    endlabel_T	*el_اگلا;	    // chain end_label locations
    صحیح		el_انتہا_label;	    // instruction idx where to set end
};

/*
 * info specific for the scope of :if / elseif / else
 */
قسمتعریف ساخت {
    صحیح		is_نظر_آیا_else;
    صحیح		is_نظر_آیا_skip_not;   // a block was unconditionally executed
    صحیح		is_had_return;	    // every block ends in :return
    صحیح		is_if_label;	    // instruction idx at IF or ELSEIF
    endlabel_T	*is_انتہا_label;	    // instructions to set end label
} ifscope_T;

// info used by :for and :while needed for ENDLOOP
قسمتعریف ساخت {
    صحیح	    li_local_گنتی;	    // ctx_locals.ga_len at loop start
    صحیح	    li_احاطہ_گنتی;	    // ctx_closure_count at loop start
    صحیح	    li_funcref_idx;	    // index of var that holds funcref count
    صحیح	    li_گہرائی;		    // nested loop depth
} loop_info_T;

/*
 * info specific for the scope of :while
 */
قسمتعریف ساخت {
    صحیح		ws_چوٹی_label;	    // instruction idx at WHILE
    endlabel_T	*ws_انتہا_label;	    // instructions to set end
    loop_info_T ws_loop_info;	    // info for LOOPEND
} whilescope_T;

/*
 * info specific for the scope of :for
 */
قسمتعریف ساخت {
    صحیح		fs_چوٹی_label;	    // instruction idx at FOR
    endlabel_T	*fs_انتہا_label;	    // break instructions
    loop_info_T	fs_loop_info;	    // info for LOOPEND
} forscope_T;

/*
 * info specific for the scope of :try
 */
قسمتعریف ساخت {
    صحیح		ts_try_label;	    // instruction idx at TRY
    endlabel_T	*ts_انتہا_label;	    // jump to :finally or :endtry
    صحیح		ts_catch_label;	    // instruction idx of last CATCH
    صحیح		ts_caught_all;	    // "catch" without argument encountered
    صحیح		ts_has_finally;	    // "finally" encountered
    صحیح		ts_no_return;	    // one of the blocks did not end in return
} tryscope_T;

قسمتعریف شمار {
    NO_SCOPE,
    IF_SCOPE,
    WHILE_SCOPE,
    FOR_SCOPE,
    TRY_SCOPE,
    بلاک_SCOPE
} scopetype_T;

/*
 * Info for one scope, pointed to by "ctx_scope".
 */
قسمتعریف ساخت scope_S scope_T;
ساخت scope_S {
    scope_T	*se_outer;	    // scope containing this one
    scopetype_T se_type;
    صحیح		se_local_گنتی;	    // ctx_locals.ga_len before scope
    skip_T	se_skip_save;	    // ctx_skip before the block
    صحیح		se_loop_گہرائی;	    // number of loop scopes, including this
    اتحاد {
	ifscope_T	se_if;
	whilescope_T	se_while;
	forscope_T	se_for;
	tryscope_T	se_try;
    } se_u;
};

/*
 * Entry for "ctx_locals".  Used for arguments and local variables.
 */
قسمتعریف ساخت {
    char_u	*lv_name;
    type_T	*lv_type;
    صحیح		lv_idx;		// index of the variable on the stack
    صحیح		lv_loop_گہرائی;	// depth for variable inside a loop or -1
    صحیح		lv_loop_idx;	// index of first variable inside a loop or -1
    صحیح		lv_from_outer;	// nesting level, using ctx_outer scope
    صحیح		lv_const;	// ASSIGN_VAR (can be assigned to),
				// ASSIGN_FINAL (no assignment) or ASSIGN_CONST
				// (value cannot be changed)
    صحیح		lv_arg;		// when TRUE this is an argument
    ufunc_T	*lv_ufunc;	// for a nested function: its ufunc, see
				// compile_nested_generic_func()
} lvar_T;

// Destination for an assignment or ":unlet" with an index.
قسمتعریف شمار {
    dest_local,
    dest_option,
    dest_func_option,
    dest_env,
    dest_global,
    dest_بفر,
    dest_کھڑکی,
    dest_tab,
    dest_vimvar,
    dest_class_member,
    dest_script,
    dest_script_v9,
    dest_reg,
    dest_اظہاریہ,
} تفویض_dest_T;

// Used by compile_lhs() to store information about the LHS of an assignment
// and one argument of ":unlet" with an index.
قسمتعریف ساخت {
    تفویض_dest_T   lhs_dest;	    // type of destination

    char_u	    *lhs_name;	    // allocated name excluding the last
				    // "[expr]" or ".name".
    size_t	    lhs_varlen;	    // length of the variable without
				    // "[expr]" or ".name"
    char_u	    *lhs_پورا;	    // allocated name including the last
				    // "[expr]" or ".name" for :redir
    size_t	    lhs_varlen_کل; // length of the variable including
				      // any "[expr]" or ".name"
    char_u	    *lhs_dest_انتہا;  // end of the destination, including
				    // "[expr]" or ".name".
    char_u	    *lhs_انتہا;	    // end including any type

    صحیح		    lhs_has_اشاریہ;  // has "[expr]" or ".name"

    صحیح		    lhs_new_local;  // create new local variable
    صحیح		    lhs_opt_flags;  // for when destination is an option
    صحیح		    lhs_vimvaridx;  // for when destination is a v:var

    lvar_T	    lhs_local_lvar; // used for existing local destination
    lvar_T	    lhs_arg_lvar;   // used for argument destination
    lvar_T	    *lhs_lvar;	    // points to destination lvar

    class_T	    *lhs_class;		    // for dest_class_member
    صحیح		    lhs_classmember_idx;    // for dest_class_member

    صحیح		    lhs_scriptvar_sid;
    صحیح		    lhs_scriptvar_idx;

    صحیح		    lhs_has_type;   // type was specified
    type_T	    *lhs_type;
    صحیح		    lhs_member_idx;    // object member index
    type_T	    *lhs_member_type;  // list/dict/object member type

    صحیح		    lhs_append;	    // used by ISN_REDIREND
} lhs_T;

/*
 * Context for compiling lines of a :def function.
 * Stores info about the local variables and condition stack.
 */
ساخت cctx_S {
    ufunc_T	*ctx_ufunc;	    // current function
    صحیح		ctx_lnum;	    // line number in current function
    char_u	*ctx_سطر_آغاز;    // start of current line or NULL
    garray_T	ctx_instr;	    // generated instructions

    صحیح		ctx_پچھلا_lnum;	    // line number below previous command, for
				    // debugging

    compiletype_T ctx_compile_type;

    garray_T	ctx_locals;	    // currently visible local variables

    صحیح		ctx_has_احاطہ;    // set to one if a FUNCREF was used in the
				    // function
    صحیح		ctx_احاطہ_گنتی;  // incremented for each closure created in
				    // the function.

    skip_T	ctx_skip;
    scope_T	*ctx_scope;	    // current scope, NULL at toplevel
    صحیح		ctx_had_return;	    // last seen statement was "return"
    صحیح		ctx_had_throw;	    // last seen statement was "throw"

    cctx_T	*ctx_outer;	    // outer scope for lambda or nested
				    // function
    صحیح		ctx_outer_مستعمل;	    // var in ctx_outer was used

    garray_T	ctx_type_ڈھیر;	    // type of each item on the stack
    garray_T	*ctx_type_list;	    // list of pointers to allocated types

    صحیح		ctx_has_cmdmod;	    // ISN_CMDMOD was generated

    lhs_T	ctx_redir_lhs;	    // LHS for ":redir => var", valid when
				    // lhs_name is not NULL
};

/*
 * List of special functions for "compile_arguments()".
 */
قسمتعریف شمار {
    CA_NOT_SPECIAL,
    CA_SEARCHPAIR,	    // {skip} in searchpair() and searchpairpos()
    CA_SUBSTITUTE,	    // {sub} in substitute(), when prefixed with \=
} ca_special_T;

// flags for typval2type()
#تعریف_کرو TVTT_DO_MEMBER	    1
#تعریف_کرو TVTT_مزید_SPECIFIC  2	// get most specific type for member

// flags for call_def_function()
#تعریف_کرو DEF_USE_PT_ARGV	    1	// use the partial arguments

// Flag used for conversion to string by may_generate_2STRING()
#تعریف_کرو TOSTRING_NONE		0x0
// Convert a List to series of values separated by newline
#تعریف_کرو TOSTRING_INTERPOLATE	0x1
// Convert a List to a textual representation of the list "[...]"
#تعریف_کرو TOSTRING_TOLERANT	0x2
