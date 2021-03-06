#include "work_with_tree.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "operation_codes.h"
#include "warnings.h"

#define IF_DEBUG_PARSING(code) //code

const int BIG_NUMBER = 16;

struct Parser {
	const char* pointer;

  private:
  	void skip_spaces() {
  		while(isspace(*pointer))
  			++pointer;
  	}

  	void syntax_error() {
  		warning("Syntax error", INFORMATION_ABOUT_CALL);
  	}

  	bool require(char symbol) {
  		if(symbol != *pointer)
  			return false;

  		if(*pointer != '\0')
  			++pointer;
  		return true;
  	}

  	Node* get_number() {
  		IF_DEBUG_PARSING(printf("GET_N\n");)
  		char* ptr_after_number = NULL;
		double value = strtod(pointer, &ptr_after_number);

		if(pointer == ptr_after_number)
			return NULL;

		pointer = ptr_after_number;

		Node* tmp = node_construct(NUMBER, value, NULL, NULL);
		return tmp;
  	}

  	Node* get_variable() {
  		IF_DEBUG_PARSING(printf("GET_V\n");)
  		skip_spaces();
  		if('a' <= *pointer && *pointer <= 'z') {
  			Node* new_node = node_construct(VARIABLE, *pointer, NULL, NULL);
  			++pointer;
  			return new_node;
  		}

  		return NULL;
  	}

  	Node* get_function() {
  		IF_DEBUG_PARSING(printf("GET_F\n");)
  		skip_spaces();
  		Node* new_node = NULL;
  		const char* find_str = NULL;
  		
  		for(int function = SIN; function <= LG; ++function) {
  			char* buffer = (char*)calloc(BIG_NUMBER, sizeof(char));
  			strcat(buffer, (char*)TEXT_OPERATIONS[function]);  			
  			strcat(buffer, "(");

	  		find_str = strstr(pointer, buffer);

	  		if(find_str == pointer) {
	  			new_node = node_construct(OPERATOR, function, NULL, NULL);
	  			pointer += OPERATIONS_LENGTH[function] + 1;
	  			new_node->left = get_expression();
	  		} else {
	  			free(buffer);
	  			continue;
	  		}

	  		free(buffer);

	  		skip_spaces();
	  		if(!require(')')) {
	  			node_destruct(new_node);
	  			return NULL;
	  		} else
	  			break;
	  	}

  		return new_node;
  	}

  	Node* get_priority() {
  		IF_DEBUG_PARSING(printf("GET_P\n");)
  		skip_spaces();
  		Node* new_node = NULL;

  		if(*pointer == '(') {
  			++pointer;
  			new_node = get_expression();
  			if(new_node == NULL) {
  				node_destruct(new_node);
  				return NULL;
  			}

  			skip_spaces();
  			if(!require(')')) {
  				node_destruct(new_node);
  				return NULL;
  			}

  			return new_node;
  		}

  		new_node = get_number();
  		if(new_node != NULL) {
  			return new_node;
  		}

  		new_node = get_function();
  		if(new_node != NULL)
  			return new_node;

  		new_node = get_variable();
  		if(new_node != NULL)
  			return new_node;

  		node_destruct(new_node);

  		return NULL;
  	}

  	Node* get_unary() {
  		IF_DEBUG_PARSING(printf("GET_U\n");)
  		skip_spaces();

  		if(*pointer == '+')
  			++pointer;
  		else if(*pointer == '-') {
  			++pointer;
  			Node* new_node = node_construct(OPERATOR, MUL, NULL, NULL);
  			new_node->left  = node_construct(NUMBER, -1.0, NULL, NULL);
  			new_node->right = get_priority();

  			return new_node;
  		}

  		return get_priority();
  	}

  	Node* get_degree() {
  		IF_DEBUG_PARSING(printf("GET_D\n");)
	  	skip_spaces();
	  	Node* left_son = get_unary();
	  	skip_spaces();

	  	if(left_son == NULL) {
	  		node_destruct(left_son);
	  		return NULL;
	  	}

	  	if(*pointer != '^')
	  		return left_son;

	  	Node* new_node = node_construct(OPERATOR, POW, NULL, NULL);
	  	node_make_copy(left_son, new_node->left);
	  	free(left_son);
	  	++pointer;
	  	new_node->right = get_degree();
	  	return new_node;
  	}

  	Node* get_term() {
  		IF_DEBUG_PARSING(printf("GET_T\n");)
  		skip_spaces();
  		Node* left_son = get_degree();
  		skip_spaces();

  		if(left_son == NULL)
  			return NULL;

  		int code_operation = 0;

  		if(*pointer == '*')
  			code_operation = MUL;
  		else if(*pointer == '/')
  			code_operation = DIV;
  		else
  			return left_son;

  		Node* new_node = node_construct(OPERATOR, code_operation, NULL, NULL);
  	  	node_make_copy(left_son, new_node->left);
	  	free(left_son);
  		++pointer;
  		new_node->right = get_term();

  		return new_node;
  	}

  	Node* get_expression() {
  		IF_DEBUG_PARSING(printf("GET_E\n");)
  		skip_spaces();
  		Node* left_son = get_term();
  		skip_spaces();

  		if(left_son == NULL)
  			return NULL;

  		int code_operation = 0;
  		if(*pointer == '+')
  			code_operation = ADD;
  		else if(*pointer == '-')
  			code_operation = SUB;
  		else
  			return left_son;

  		Node* new_node = node_construct(OPERATOR, code_operation, NULL, NULL);
  	  	node_make_copy(left_son, new_node->left);
	  	free(left_son);
  		++pointer;
  		new_node->right = get_expression();

  		return new_node;
  	}

  public:
  	void construct() {
  		pointer = NULL;
  	}

  	Node* get_G(const char* line_to_parsing) {
  		IF_DEBUG_PARSING(printf("GET_G\n");)
  		if(line_to_parsing == NULL) {
  			warning("Bad string to parsing", base_arguments_of_call);
  			return NULL;
  		}

  		pointer = line_to_parsing;
  		Node* root = get_expression();

  		skip_spaces();
  		if(!require('\0') || root == NULL) {
  			syntax_error();
  			node_destruct(root);
  			return NULL;
  		}

  		return root;
  	}
};

void tree_fill(Tree* tree, const char* name_file);
void get_variables(Tree* tree);
void get_variables(Node* node, bool* used_vars);