#include <stdio.h>
#include "parsing.h"
#include "work_with_tree.h"
#include "take_derivate.h"
#include "make_latex.h"

#define IF_DEBUG_DERIVATE(code) code

#define CHECK_LATEX_FILE(node)							\
	if(!latex)											\
		return node;

#define CHECK_NODE(node)								\
	if(!node)											\
		return node;

#define CHECK_TREE(tree)								\
	if(!tree)											\
		return;

#define CHECK_CHILDREN(node)							\
	if(!node->left || !node->right)						\
		return node;

#define FREE_NODE(node)									\
	free(node->left);									\
	free(node->right);									\
	free(node);											


void derivate_tree(Tree* tree, FILE* latex) {
	CHECK_TREE(tree)

	if(!latex)
		return;

	printf("begin derivate\n");
	tree->root = derivate_node(tree->root, latex);
	printf("end derivate\n\n\n");
}

Node* derivate_node(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	if(node->type == NUMBER) {
		fprintf(latex, "Derivatives of a number are easy! Look at it: \n$$ ");
		latex_node(node, latex, false);
		fprintf(latex, "' = 0 $$\n");
		node->value = 0;
	} else if(node->type == OPERATOR) {
		Node* old_node = node_construct(node->type, node->value, NULL, NULL);
		node_make_copy(node, old_node);

		fprintf(latex, "This expression is quite complex. Let's try to differentiate it piece by piece: \n$$ (");
		latex_node(node, latex, true);
		fprintf(latex, ")'$$\n");

		if(node->value == ADD || node->value == SUB) 
			node = derivate_add_and_sub(node, latex);

		else
		if(node->value == MUL) 
			node = derivate_mul(node, latex);
		
		else
		if(node->value == DIV)
			node = derivate_div(node, latex);

		else
		if(node->value == POW)
			node = derivate_pow(node, latex);

		else
		if(node->value == SIN) 
			node = derivate_sin(node, latex);
	
		else
		if(node->value == COS) 
			node = derivate_cos(node, latex);

		else
		if(node->value == LG)
			node = derivate_lg(node, latex);

		else
		if(node->value == LN)
			node = derivate_ln(node, latex);

		else
		if(node->value == TG || node->value == CTG)
			node = derivate_tg_and_ctg(node, latex, (OPERATION_CODES)node->value);

		else
		if(node->value == ARCSIN || node->value == ARCCOS)
			node = derivate_arcsin_and_arccos(node, latex, (OPERATION_CODES)node->value);

		else
		if(node->value == ARCTG || node->value == ARCCTG)
			node = derivate_arctg_and_arcctg(node, latex, (OPERATION_CODES)node->value);

		fprintf(latex, "Come back to our expression: \n$$ (");
		latex_node(old_node, latex, true);
		fprintf(latex, ")' = \n");
		latex_node(node, latex, false);
		fprintf(latex, " $$\n");

		FREE_NODE(old_node)

	} else {
		if(node->value == 'e') {
			fprintf(latex, "e is a number, so its derivative is extremely simple: \n$$ e' = 0 $$\n");
			node->value = 0;
		}
		else {
			fprintf(latex, "Variable is fairly easy to work with when differentiating: \n$$ %c' = 1 $$\n", (int)node->value);
			node->value = 1;
		}
		node->type = NUMBER;
	}

	return node;
}

static Node* derivate_add_and_sub(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	node->left  = derivate_node(node->left, latex);
	node->right = derivate_node(node->right, latex);

	return node;
}

static Node* derivate_mul(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)
	CHECK_CHILDREN(node)

	return take_derivate_from_children_and_call_func(node, latex, MUL);
}

static inline Node* do_derivating_mul(Node* left, Node* right, Node* der_left, Node* der_right, FILE* latex) {
	Node* new_left  = node_construct(OPERATOR, MUL, NULL,  NULL);
	Node* new_right = node_construct(OPERATOR, MUL, NULL, NULL);

	node_make_copy(left,  new_left->left);
	node_make_copy(right, new_right->left);

	node_make_copy(der_right, new_left->right);
	node_make_copy(der_left, new_right->right);		

	Node* result_derivating = node_construct(OPERATOR, ADD, NULL, NULL);

	node_make_copy(new_left,  result_derivating->left);
	node_make_copy(new_right, result_derivating->right);

	FREE_NODE(new_left)
	FREE_NODE(new_right)

	return result_derivating;
}

static Node* derivate_div(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	return take_derivate_from_children_and_call_func(node, latex, DIV);
}

static inline Node* do_derivating_div(Node* left, Node* right, Node* der_left, Node* der_right, FILE* latex) {
	//---------------------------------------- make left -------------------------------------------

	Node* answer_left = node_construct(OPERATOR, SUB, NULL, NULL);

	Node* answer_left_left = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(der_left, answer_left_left->left);
	node_make_copy(right,    answer_left_left->right);

	Node* answer_left_right = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(left,      answer_left_right->left);
	node_make_copy(der_right, answer_left_right->right);

	node_make_copy(answer_left_left,  answer_left->left);
	node_make_copy(answer_left_right, answer_left->right);

	//---------------------------------------- make right -------------------------------------------

	Node* answer_right = node_construct(OPERATOR, POW, NULL, NULL);

	node_make_copy(right, answer_right->left);
	
	Node* answer_right_right = node_construct(NUMBER, 2, NULL, NULL);
	node_make_copy(answer_right_right, answer_right->right);

	test_latex(answer_left, latex, "div answer left: ");
	test_latex(answer_right, latex, "div answer right: ");

	//---------------------------------------- make answer -------------------------------------------

	Node* answer = node_construct(OPERATOR, DIV, NULL, NULL);
	node_make_copy(answer_left,  answer->left);
	node_make_copy(answer_right, answer->right);

	FREE_NODE(answer_left)
	FREE_NODE(answer_left_left)
	FREE_NODE(answer_left_right)
	FREE_NODE(answer_right)			          
	FREE_NODE(answer_right_right)

	return answer;
}

static Node* derivate_pow(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)
	CHECK_CHILDREN(node)

	return take_derivate_from_children_and_call_func(node, latex, POW);
}

static inline Node* do_derivating_pow(Node* left, Node* right, Node* der_left, Node* der_right, FILE* latex) {
	//---------------------------------------- make left -------------------------------------------

	Node* answer_left = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(der_left, answer_left->right);

	Node* answer_left_left = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(right, answer_left_left->left);

	Node* answer_left_left_right = node_construct(OPERATOR, POW, NULL, NULL);
	node_make_copy(left, answer_left_left_right->left);

	Node* answer_left_left_right_right = node_construct(OPERATOR, SUB, NULL, NULL);
	node_make_copy(right, answer_left_left_right_right->left);
	answer_left_left_right_right->right = node_construct(NUMBER, 1, NULL, NULL);

	node_make_copy(answer_left_left_right_right, answer_left_left_right->right);

	node_make_copy(answer_left_left_right, answer_left_left->right);

	node_make_copy(answer_left_left, answer_left->left);

	//---------------------------------------- make right -------------------------------------------

	Node* answer_right = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(der_right, answer_right->right);

	Node* answer_right_left = node_construct(OPERATOR, MUL, NULL, NULL);

	Node* answer_right_left_left = node_construct(OPERATOR, LN, NULL, NULL);
	node_make_copy(left, answer_right_left_left->left);

	node_make_copy(answer_right_left_left, answer_right_left->left);

	Node* answer_right_left_right = node_construct(OPERATOR, POW, NULL, NULL);
	node_make_copy(left,  answer_right_left_right->left);
	node_make_copy(right, answer_right_left_right->right);

	node_make_copy(answer_right_left_right, answer_right_left->right);

	node_make_copy(answer_right_left, answer_right->left);

	//---------------------------------------- make answer -------------------------------------------

	Node* answer = node_construct(OPERATOR, ADD, NULL, NULL);
	node_make_copy(answer_left,  answer->left);
	node_make_copy(answer_right, answer->right);

	FREE_NODE(answer_left)
	FREE_NODE(answer_left_left)
	FREE_NODE(answer_left_left_right)
	FREE_NODE(answer_left_left_right_right)
	FREE_NODE(answer_right)
	FREE_NODE(answer_right_left)
	FREE_NODE(answer_right_left_left)
	FREE_NODE(answer_right_left_right)

	return answer;
}


static Node* derivate_sin(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	Node* answer = node_construct(OPERATOR, MUL, NULL, NULL);

	node_make_copy(node, answer->left);
	answer->left->value = COS;

	Node* der_arg = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(node->left, der_arg);

	der_arg = derivate_node(der_arg, latex);
	node_make_copy(der_arg, answer->right);

	FREE_NODE(der_arg)

	return answer;
}

static Node* derivate_cos(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	Node* answer_without_minus = node_construct(OPERATOR, MUL, NULL, NULL);

	node_make_copy(node, answer_without_minus->left);
	answer_without_minus->left->value = SIN;

	Node* der_arg = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(node->left, der_arg);

	der_arg = derivate_node(der_arg, latex);
	node_make_copy(der_arg, answer_without_minus->right);

	test_latex(answer_without_minus, latex, "answer_without_minus from cos: ");

	Node* answer = node_construct(OPERATOR, MUL, NULL, NULL);
	answer->left = node_construct(NUMBER, -1, NULL, NULL);
	node_make_copy(answer_without_minus, answer->right);

	FREE_NODE(answer_without_minus)
	FREE_NODE(der_arg)

	return answer;
}

static Node* derivate_tg_and_ctg(Node* node, FILE* latex, const OPERATION_CODES now_operation) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	Node* answer_left = node_construct(OPERATOR, DIV, NULL, NULL);

	Node* answer_left_left  = node_construct(NUMBER,   1,   NULL, NULL);
	if(now_operation == CTG)
		answer_left_left->value = -1;

	Node* answer_left_right = node_construct(OPERATOR, POW, NULL, NULL);

	Node* answer_left_right_left = node_construct(OPERATOR, COS, NULL, NULL);
	if(now_operation == CTG)
		answer_left_right_left->value = SIN;

	node_make_copy(node->left, answer_left_right_left->left);

	Node* answer_left_right_right = node_construct(NUMBER, 2, NULL, NULL);

	node_make_copy(answer_left_right_right, answer_left_right->right);
	node_make_copy(answer_left_right_left,  answer_left_right->left);

	node_make_copy(answer_left_left,  answer_left->left);
	node_make_copy(answer_left_right, answer_left->right);

	Node* answer_right = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(node->left, answer_right);
	answer_right = derivate_node(answer_right, latex);

	Node* answer = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(answer_left, answer->left);
	node_make_copy(answer_right, answer->right);

	FREE_NODE(answer_left)
	FREE_NODE(answer_left_left)
	FREE_NODE(answer_left_right)
	FREE_NODE(answer_left_right_left)
	FREE_NODE(answer_left_right_right)
	FREE_NODE(answer_right)

	return answer;
}

static Node* derivate_arcsin_and_arccos(Node* node, FILE* latex, const OPERATION_CODES now_operation) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	Node* answer_left = node_construct(OPERATOR, DIV, NULL, NULL);
	Node* answer_left_left = node_construct(NUMBER, 1, NULL, NULL);

	if(now_operation == ARCCOS)
		answer_left_left->value = -1;

	Node* answer_left_right = node_construct(OPERATOR, POW, NULL, NULL);
	Node* answer_left_right_right = node_construct(NUMBER, 0.5, NULL, NULL);

	Node* answer_left_right_left = node_construct(OPERATOR, SUB, NULL, NULL);
	Node* answer_left_right_left_left = node_construct(NUMBER, 1, NULL, NULL);

	Node* answer_left_right_left_right = node_construct(OPERATOR, POW, NULL, NULL);
	Node* answer_left_right_left_right_left = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(node->left, answer_left_right_left_right_left);

	Node* answer_left_right_left_right_right = node_construct(NUMBER, 2, NULL, NULL);

	node_make_copy(answer_left_right_left_right_left,  answer_left_right_left_right->left);
	node_make_copy(answer_left_right_left_right_right, answer_left_right_left_right->right);

	node_make_copy(answer_left_right_left_left,  answer_left_right_left->left);
	node_make_copy(answer_left_right_left_right, answer_left_right_left->right);

	node_make_copy(answer_left_right_left, answer_left_right->left);
	node_make_copy(answer_left_right_right, answer_left_right->right);

	node_make_copy(answer_left_left,  answer_left->left);
	node_make_copy(answer_left_right, answer_left->right);

	Node* der_arg = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(node->left, der_arg);
	der_arg = derivate_node(der_arg, latex);

	Node* answer = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(answer_left, answer->left);
	node_make_copy(der_arg,     answer->right);

	FREE_NODE(answer_left)
	FREE_NODE(answer_left_left)
	FREE_NODE(answer_left_right)
	FREE_NODE(answer_left_right_right)
	FREE_NODE(answer_left_right_left)
	FREE_NODE(answer_left_right_left_left)
	FREE_NODE(answer_left_right_left_right)
	FREE_NODE(answer_left_right_left_right_left)
	FREE_NODE(answer_left_right_left_right_right)
	FREE_NODE(der_arg)

	return answer;
}

static Node* derivate_arctg_and_arcctg(Node* node, FILE* latex, const OPERATION_CODES now_operation) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	Node* answer_left = node_construct(OPERATOR, DIV, NULL, NULL);
	Node* answer_left_left = node_construct(NUMBER, 1, NULL, NULL);

	if(now_operation == ARCCTG)
		answer_left_left->value = -1;

	Node* answer_left_right = node_construct(OPERATOR, ADD, NULL, NULL);
	Node* answer_left_right_left = node_construct(NUMBER, 1, NULL, NULL);

	Node* answer_left_right_right = node_construct(OPERATOR, POW, NULL, NULL);
	Node* answer_left_right_right_left = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(node->left, answer_left_right_right_left);

	Node* answer_left_right_right_right = node_construct(NUMBER, 2, NULL, NULL);

	node_make_copy(answer_left_right_right_left,  answer_left_right_right->left);
	node_make_copy(answer_left_right_right_right, answer_left_right_right->right);	

	node_make_copy(answer_left_right_left,  answer_left_right->left);
	node_make_copy(answer_left_right_right, answer_left_right->right);

	node_make_copy(answer_left_left,  answer_left->left);
	node_make_copy(answer_left_right, answer_left->right);

	Node* der_arg = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(node->left, der_arg);
	der_arg = derivate_node(der_arg, latex);

	Node* answer = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(answer_left, answer->left);
	node_make_copy(der_arg,     answer->right);

	FREE_NODE(answer_left)
	FREE_NODE(answer_left_left)
	FREE_NODE(answer_left_right)
	FREE_NODE(answer_left_right_left)
	FREE_NODE(answer_left_right_right)
	FREE_NODE(answer_left_right_right_left)
	FREE_NODE(answer_left_right_right_right)
	FREE_NODE(der_arg)

	return answer;
}

static Node* derivate_ln(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	Node* old_arg = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(node->left, old_arg);

	Node* answer_left 	   = node_construct(OPERATOR, DIV, NULL, NULL);
	Node* answer_left_left = node_construct(NUMBER, 1, NULL, NULL);
	node_make_copy(answer_left_left, answer_left->left);
	node_make_copy(old_arg, 		 answer_left->right);

	Node* der_old_arg = node_construct(node->left->type, node->left->value, NULL, NULL);
	node_make_copy(old_arg, der_old_arg);
	der_old_arg = derivate_node(der_old_arg, latex);

	Node* answer = node_construct(OPERATOR, MUL, NULL, NULL);
	node_make_copy(answer_left, answer->left);
	node_make_copy(der_old_arg, answer->right);

	FREE_NODE(old_arg)
	FREE_NODE(answer_left)
	FREE_NODE(answer_left_left)
	FREE_NODE(der_old_arg)

	return answer;
}

static Node* derivate_lg(Node* node, FILE* latex) {
	CHECK_LATEX_FILE(node)
	CHECK_NODE(node)

	Node* get_der_ln = derivate_ln(node, latex);
	Node* answer_right = node_construct(NUMBER, LG_10, NULL, NULL);

	Node* answer = node_construct(OPERATOR, DIV, NULL, NULL);
	node_make_copy(get_der_ln, answer->left);
	node_make_copy(answer_right, answer->right);

	FREE_NODE(get_der_ln)
	FREE_NODE(answer_right)

	return answer;
}


bool is_leaf(Node* node) {
	if(!node || !node->left && !node->right)
		return true;
	return false;
}

static inline Node* take_derivate_from_children_and_call_func(Node* node, FILE* latex, const OPERATION_CODES now_operation) {
	Node* old_left  = node_construct(node->left->type,  node->left->value,  NULL, NULL);
	Node* old_right = node_construct(node->right->type, node->right->value, NULL, NULL);

	node_make_copy(node->left,  old_left);
	node_make_copy(node->right, old_right);

	Node* der_old_left  = node_construct(old_left->type,  old_left->value,  NULL, NULL);
	Node* der_old_right = node_construct(old_right->type, old_right->value, NULL, NULL);

	node_make_copy(old_left, der_old_left);
	node_make_copy(old_right, der_old_right);

	der_old_left  = derivate_node(der_old_left,  latex);
	der_old_right = derivate_node(der_old_right, latex);

	Node* answer = NULL;

	if(now_operation == POW) 
		node_make_copy(do_derivating_pow(old_left, old_right, der_old_left, der_old_right, latex), answer);
	else
	if(now_operation == DIV)
		node_make_copy(do_derivating_div(old_left, old_right, der_old_left, der_old_right, latex), answer);

	else
	if(now_operation == MUL)
		node_make_copy(do_derivating_mul(old_left, old_right, der_old_left, der_old_right, latex), answer);

	FREE_NODE(old_left)
	FREE_NODE(old_right)
	FREE_NODE(der_old_left)
	FREE_NODE(der_old_right)

	return answer;
}
