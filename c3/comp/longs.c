/*
    Modification history for longs.c:
        24-May-83       Make *node in tranlexp be register variable
                        for optimal code size.
        13-Jul-83       Long constant node freed before use when
                        ptr to long is UREG.  Also, optimized push
                        of zero.
        14-Sep-83 LAC   add register content tracking code.
        13-Apr-84 LAC   Conversion for UNIX.
*/

#include "cj.h"

#define swap(x,y)      {expnode *t;t=x;x=y;y=t;}

lload(ptr)
expnode *ptr;
{
    tranlexp(ptr);
    getadd(ptr);
}

tranlexp(node)
register expnode *node;
{
    register expnode *p;
    register int op,s;
    long l;

    switch (op = node->op) {
        case STAR:
            dostar(node);
            getinx(node);
            switch (node->op) {
                case XIND:
                case YIND:
                case UIND:
                    if (node->val.num) {
                        gen(LEAX,NODE,node);
                        node->op = XIND;
                        node->val.num = 0;
                    }
            }
            break;
        case UTOL:
            lddexp(node->left);
            gen(LONGOP,UTOL);
            node->op=FREG;
            break;
#ifdef  DOFLOATS
        case DTOL:
            dload(node->left);
            gen(DBLOP,DTOL,node->left);
            node->op=FREG;
            break;
#endif
        case ITOL:
            lddexp(node->left);
            gen(LONGOP,ITOL);
            node->op=FREG;
            break;
        case LCONST:
#ifdef DEBUG
fprintf(stderr,"tranlexp: *(node->val.lp=%04X)=%08lX\n",
node->val.lp,*node->val.lp);
#endif
            gen(LONGOP,LCONST,node->val.lp);
            /*  should free constant storage here  */
            node->val.lp = NULL;
			node->op = XIND;
			node->val.num = 0;
			break;
        case QUERY:
            doquery(node,lload);
			node->op = XIND;
			node->val.num = 0;
			break;
        case INCBEF:
        case DECBEF:
        case COMPL:
        case NEG:
            lload(node->left);
            gen(LONGOP,op);
			node->op = XIND;
			node->val.num = 0;
			break;
        case INCAFT:
        case DECAFT:
            gen(LOADIM,XREG,FREG,0);
            gen(PUSH,XREG);
            lload(node->left);
            gen(LONGOP,op);
            gen(LONGOP,MOVE);
            gen(LONGOP,op==INCAFT ? DECAFT : INCAFT);
            node->op=FREG;
            break;
        case CALL:
            docall(node);
            node->op=FREG;
            break;
        case TIMES:
            if (node->left->op == LCONST) swap(node->left,node->right);
            if (node->right->op == LCONST) {
                l = *node->right->val.lp;
                if (l > 0 && l < 65536 && (s = isashift((int) l))){
                    p = node->right;
                    /*  should free constant storage here  */
                    p->val.num = s;
                    p->op = CONST;
                    p->type = INT;
                    op = (op == TIMES) ? SHL : SHR;
                    goto shifts;
                }
            }
        case DIV:
        case EQ:
        case NEQ:
        case GEQ:
        case LEQ:
        case GT:
        case LT:
        case PLUS:
        case MINUS:
        case MOD:
        case AND:
        case OR:
        case XOR:
            p=node->left;
            if(p->op==LCONST) pushcon(p);
            else {
                lload(p);
                gen(LONGOP,STACK);
            }
            lload(node->right);
            gen(LONGOP,op);
            node->op=FREG;
            break;

shifts:
        case SHL:
        case SHR:
            lload(node->left);
            gen(PUSH,XREG);
            lddexp(node->right);
            gen(LONGOP,op);
            node->op=FREG;
            break;

        case ASSIGN:
            lload(node->left);
            gen(PUSH,XREG);
            lload(node->right);
			gen(LONGOP,MOVE);
			node->op = XIND;
			node->val.num = 0;
			break;

        case FREG:
        case XIND:
        case YIND:
        case UIND:
        case NAME:
            return;

        default:
            if (op >= ASSPLUS) {
				lload(p=node->left);
				gen(PUSH,XREG);
				node->op=op-(ASSPLUS-PLUS);
				p->op=XIND;
				tranlexp(node);
				gen(LONGOP,MOVE);
				node->op = XIND;
				node->val.num = 0;
				break;
			}
            comperr(node,"longs");
    }
    clrconts();          /* clear D contents */
}

getadd(ptr)
register expnode *ptr;
{
    switch(ptr->op) {
        case NAME:
            gen(LOADIM,XREG,NODE,ptr);
            setxreg(ptr);
            break;
        case YIND:
        case UIND:
            gen(LEAX,NODE,ptr);
            setxreg(ptr);
            break;
    }
}

pushcon(p)
register expnode *p;
{
    register long *p1;

    if ((p1 = p->val.lp) && *p1) {
        gen(LOAD,DREG,CONST,(int) *p1);
        gen(PUSH,DREG);
        gen(LOAD,DREG,CONST,(int) (*p1 >> 16));
        gen(PUSH,DREG);
    } else {
        gen(LOAD,DREG,CONST,0);
        gen(PUSH,DREG);
        gen(PUSH,DREG);
    }
    if (p1) {
        /*  should free constant storage here  */
        p->val.lp = NULL;
    }
}

