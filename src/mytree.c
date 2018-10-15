/**
 * @file 	mytree.c
 * @brief	Ficheiro contendo funções utilizadas na construção da AVL utilizada no programa bem como todas as funcionalidades pela mesma suportadas.
 */
#include "mytree.h"


#define MAX(a,b) a > b ? a : b;
#define MAX_SIZE 40

struct AVBin {
    int altura;
    void * key;
	void * data;
    struct AVBin * esq, * dir;
};

typedef struct AVBin * AVL;

struct tree{
    AVL arv;
    long nnodes;
	void * (*replace_fun)(void *,void *);
    int (*f_compare)(void *,void *);
	void (*destroy_key)(void *);
	void (*destroy_data)(void *);
    int heigth;
};

/**
 * @brief			Função calcula a altura de um nodo.
 * @param a			Apontador para a árvore.
 * @return 			Altura de um nodo.
*/
static int altura(AVL a) {
    return a ? a-> altura : 0;
}

/**
 * @brief			Função calcula o balanço de um nodo.
 * @param a			Apontador para a árvore.
 * @return 			Inteiro com o valor do balanço.
*/
static int balanceDEEP(AVL a) {
    int balance = 0;
    if (a){
        balance = altura(a->dir) - altura(a->esq);
    }

    return balance;
}

/**
 * @brief			Função calcula a altura de uma árvore.
 * @param a			Apontador para a árvore.
 * @return 			Altura da árvore.
*/
static int cal_altura(AVL a){
    int h = 0;
    int h1, h2;
    if(a){
        h1 = cal_altura(a->esq);
        h2 = cal_altura(a->dir);

        h = h1 > h2 ? h1 + 1 : h2 +1;
    }

    return h;
}

/**
 * @brief			Função verifica se a árvore é balanceada.
 * @param	a		Apontador para a árvore.
 * @return 			Inteiro a ser usado como boolean.
*/
static int isBalanced(AVL a){
    int r = 1;
    int b;
    if(a){
        b = cal_altura(a->dir) - cal_altura(a->esq);
        if ((b >= -1) && (b <= 1))
            r = isBalanced(a->esq) && isBalanced(a->dir);
        else r = 0;
    }

    return r;
}

/**
 * @brief			Função verifica se a arvore da estrutura é balanceada.
 * @param tree		Apontador para a estrutura.
 * @return 			Inteiro a ser usado como boolean.
*/
int TREE_balance(TREE tre){
	return isBalanced(tre->arv);
}

/**
 * @brief			Função efetua uma rotação para a direita da árvore.
 * @param	a		Apontador para a árvore.
 * @return 			Árvore após ser rodada para a direita.
*/
static AVL rotate_rigth(AVL a){

    int ha_r, ha_l;

    AVL aux = a -> esq;
    a->esq = aux -> dir;
    aux -> dir = a;


    ha_r = altura(a->dir);
    ha_l = altura(a->esq);
    a->altura = ha_r > ha_l ? ha_r + 1 : ha_l + 1;

    ha_l = altura(aux->esq);
    ha_r = a->altura;
    aux->altura = ha_r > ha_l ? ha_r + 1 : ha_l + 1;

    return aux;
}

/**
 * @brief			Função efetua uma rotação para a esquerda da árvore.
 * @param	a		Apontador para a árvore.
 * @return 			Árvore após ser rodada para a esquerda.
*/
static AVL rotate_left(AVL a){

    int ha_r, ha_l;

    AVL aux = a->dir;
    a->dir = aux -> esq;
    aux->esq = a;

    ha_l = altura(a->esq);
    ha_r = altura(a->dir);
    a->altura = ha_r > ha_l ? ha_r + 1 : ha_l + 1;

    ha_r = altura(aux->dir);
    ha_l = a->altura;
    aux->altura = ha_r > ha_l ? ha_r + 1 : ha_l + 1;


    return aux;
}

/**
 * @brief			Função efetua o balanceamento da árvore.
 * @param	a		Apontador para a árvore.
 * @return 			Árvore balanceada.
*/
static AVL balance(AVL a){

    int hd, hl;

    hd = altura(a->dir);
    hl = altura(a->esq);

    int bal = hd -hl;


    if (bal == -2){
        if (balanceDEEP(a->esq) == 1)
            a->esq = rotate_left(a->esq);
        a = rotate_rigth(a);
    }
    if (bal == 2){
        if (balanceDEEP(a->dir) == -1)
            a->dir = rotate_rigth(a->dir);
        a = rotate_left(a);
    }

    return a;
}

/**
 * @brief			Função que implementa a nova altura de um dado nodo.
 * @param	a		Apontador para a árvore.
*/
static void implementa_alt(AVL * a){

    int hl,hr;

    AVL aux = * a;
    hr = altura(aux->dir);
    hl = altura(aux->esq);
    aux->altura = hr > hl ? hr + 1 : hl + 1;

}

/**
 * @brief			Função que cria um novo nodo.
 * @param key		Apontador a key.
 * @param data		Apontador para a data.
 * @return 			Apontador para o nodo da árvore.
*/
static AVL create_new_node(void * key, void * data){
    AVL a;
    a = malloc(sizeof(struct AVBin));
    a -> altura = 1;
    a -> key = key;
	a -> data = data;
    a -> esq = NULL;
    a -> dir = NULL;

    return a;
}

/**
 * @brief			Função insere um elemento na árvore.
 * @param gl		Apontador para a estrutura que guarda a árvore.
 * @param key		Apontador para a key a inserir.
 * @param data		Apontador para a data a inserir.
 * @return 			Apontador para a estrutura após ser inserido o valor.
*/
TREE insere_tree(TREE gl, void * key, void * data){

    AVL queue[MAX_SIZE];
    AVL a = gl->arv;
	int replace = 0;
	int side;

    int idx = 0;
    queue[idx++] = NULL;

    if (!a){
        a = create_new_node(key,data);
		gl->arv = a;
    }
    else{
        while(1){
			side = (gl->f_compare(a->key,key));
			if (side == 0){
				if (gl->replace_fun != NULL){
					replace = 1;
					a->data=gl->replace_fun(a->data,data);
					if (gl->destroy_key != NULL)
						gl->destroy_key(key);
					break;
				}
			}
            if (side > 0){
                if (a->dir){
                    queue[idx++] = a;
                    a = a->dir;
                }
                else {
                    a->dir = create_new_node(key,data);
                    break;
                }
            }
            else {
                if (a->esq){
                    queue[idx++] = a;
                    a = a->esq;
                }
                else{
                    a->esq = create_new_node(key,data);
                    break;
                }
            }
        }

        AVL pai;
        int check_side, balan;
		if (replace == 0){
        	while(1){
            	pai = queue[--idx];
            	check_side = ((!pai) ||pai->esq == a);
            	implementa_alt(&a);
            	balan = altura(a->dir) - altura(a->esq);
            	if (balan < -1 || balan > 1){
                	a = balance(a);
                	if (!pai)
                    	break;
                	else if (check_side)
                    	pai->esq = a;
                	else pai->dir = a;
            	}
            	if(!pai)
                	break;
            	a = pai;
        	}
			gl->arv = a;
		}
    }
	gl->nnodes++;

    return gl;

}

/**
 * @brief					Função cria a estrutura que contêm a árvore.
 * @param	f_compare		Apontador para a função de comparação.
 * @param	destroy_key		Apontador para a função que dá free à key.
 * @param	destroy_data	Apontador para a função que dá free à data.
 * @param	replace			Apontador para a função que dá replace à informação.
 * @return 					Apontador para a estrutura criada.
*/
TREE createTREE(void * f_compare,void * destroy_key,void * destroy_data,void * replace){
    TREE a = malloc(sizeof(struct tree));
    a->nnodes = 0;
    a->heigth = 0;
    a->arv = NULL;
	a->replace_fun = replace;
    a->f_compare = f_compare;
	a->destroy_key = destroy_key;
	a->destroy_data = destroy_data;

    return a;
}

/**
 * @brief					Função liberta a memória de uma AVL.
 * @param	a				Apontador para a AVL.
 * @param	destroy_key		Apontador para a função que destroi a key.
 * @param	destroy_data	Apontador para a função que destroi a data.
*/
static void freeAVL(AVL a,void (*destroy_key)(void *),void (*destroy_data)(void *)){
	if (a){
		if (destroy_key != NULL)
			destroy_key(a->key);
		if (destroy_data != NULL)
			destroy_data(a->data);
		freeAVL(a->esq,destroy_key,destroy_data);
		freeAVL(a->dir,destroy_key,destroy_data);
		free(a);
	}
}





/**
 * @brief			Função liberta a memória da estrutura Tree.
 * @param	tree	Apontador para a tree.
*/
void freeTREE_AVL(TREE tre){
	if(tre){
		freeAVL(tre->arv,tre->destroy_key,tre->destroy_data);
		free(tre);
	}
}





/**
 *@brief			Função que procura um elemento na árvore.
 *@param tree		Estrutura que contém a árvore.
 *@param key		Apontador para a key a procurar.
 *@param valid		Apontador para o passar o resultado da procura.
 *@return 			Data da árvore apos ser procurado o elemento, retorna NULL caso falhe na procura.
*/
void * search_AVL(TREE tree, void * key,int * valid){
	AVL node = tree->arv;
	int result = 0;

	while((!result) && node){
		if (tree->f_compare(node->key,key) == 0){
			result = 1;
		}
		else if (tree->f_compare(node->key,key) == 1)
			node = node->dir;
		else node = node ->esq;
	}
	*valid = result;
	if (result)
		return node->data;
	return NULL;
}

/**
 *@brief			Função que testa se os nodos da AVL têm as alturas direitas
 *@param a			Apontador para a AVL.
 *@return 			Inteiro a ser usado como booelan.
*/
static int check_altura(AVL a){
    int r = 1;
    int b;
    if (a){
        b = cal_altura(a);
        if (a->altura == b){
            r = cal_altura(a->esq) && cal_altura(a->dir);
        }
    }

    return r;
}

/**
 *@brief			Função que testa a AVL é de procura.
 *@param a			Apontador para a AVL.
 *@param tree		Apontador para a estrutura com a função de comparação.
 *@return 			Inteiro a ser usado como booelan.
*/
static int isSearch(AVL a,TREE tree){
    int r = 1;
    if (a){
        if (a->dir == NULL && a->esq == NULL)
            r = 1;
        else if (a->dir == NULL && (tree->f_compare(a->key,a->esq->key) < 0))
            r = isSearch(a->esq,tree);
        else if (a->esq == NULL && (tree->f_compare(a->key,a->dir->key) > 0))
            r = isSearch(a->dir,tree);
        else if ((tree->f_compare(a->key,a->dir->key) > 0) && (tree->f_compare(a->key,a->esq->key) < 0))
            r = isSearch(a->dir,tree) && isSearch(a->esq,tree);
        else r = 0;
    }
    return r;
}

/**
 *@brief			Função que vai ser aplicada a todos os nodos.
 *@param aux		Apontador para a arvore.
 *@param f_nodo		Função a aplicar a cada nodo.
 *@param data1		Apontador a passar à função a aplicar.
*/
static void all_nodes_trans(AVL aux,void (*f_nodo)(void *,void *),void * data1){
	if (aux){
		f_nodo(aux->data,data1);
		all_nodes_trans(aux->esq,f_nodo,data1);
		all_nodes_trans(aux->dir,f_nodo,data1);
	}
}

/**
 *@brief				Função que vai ser aplicada a todos os nodos.
 *@param	e			Apontador para a estrutura que contem a AVL.
 *@param	f_nodo		Função a aplicar a cada nodo.
 *@param	data1		Apontador a passar à função a aplicar.
*/
void all_nodes_TREE(TREE e,void (*f_nodo)(void *,void *),void * data1){
	if (f_nodo != NULL)
		all_nodes_trans(e->arv,f_nodo,data1);
}

/**
 *@brief				Função que aplica uma função aos nodos que satisfazem uma condição.
 *@param	aux			Apontador para a AVL.
 *@param	inicio		Apontador para a data1 a comparar.
 *@param	fim			Apontador para a data2 a comparar.
 *@param	f_compare	Apontador para a função que compara.
 *@param	f_nodo		Função a aplicar em todos os nodos.
 *@param	data3		Aparametro 1 a passar à função que aplica nos nodos.
 *@param	data4		Aparametro 2 a passar à função que aplica nos nodos.
*/
static void all_nodes_With_key_Condition(AVL aux,void * inicio,void * fim,int (*f_compare)(void *,void *),void (*f_nodo)(void *,void *,void *),void * data3,void * data4){
	int a1,a2;
	if(aux){
		a1 =f_compare(inicio,aux->key);
		a2 =f_compare(fim,aux->key);
		if ((a1 > 0 && a2 < 0) || (a1==0) || (a2==0)){
			all_nodes_With_key_Condition(aux->esq,inicio,fim,f_compare,f_nodo,data3,data4);
			f_nodo(aux->data,data3,data4);
			all_nodes_With_key_Condition(aux->dir,inicio,fim,f_compare,f_nodo,data3,data4);
		}
		else if (a1 > 0)
			all_nodes_With_key_Condition(aux->esq,inicio,fim,f_compare,f_nodo,data3,data4);
		else if (a2 < 0)
			all_nodes_With_key_Condition(aux->dir,inicio,fim,f_compare,f_nodo,data3,data4);
	}
}

/**
 *@brief			Função que aplica uma função aos nodos que satisfazem uma condição.
 *@param tree		Apontador para a estrutura que contém a árvore.
 *@param data1		Apontador para a data1 a comparar.
 *@param data2		Apontador para a data2 a comparar.
 *@param f_nodo		Função a aplicar em todos os nodos.
 *@param data3		Aparametro 1 a passar à função que aplica nos nodos.
 *@param data4		Aparametro 2 a passar à função que aplica nos nodos.
*/
void all_nodes_With_Condition(TREE tree, void * data1, void * data2,void (*f_nodo)(void *,void *,void *),void * data3,void * data4){
	all_nodes_With_key_Condition(tree->arv,data1,data2,tree->f_compare,f_nodo,data3,data4);
}


/**
 *@brief			Função que testa as propriedas da tree.
 *@param tree		Estrutura que contém a árvore.
 *@return 			Inteiro a ser usado como booelan.
*/
int test_TREE_PROP(TREE tree){
	int alturas,procura,balanceada;
	balanceada = isBalanced(tree->arv);
	alturas = check_altura(tree->arv);
	procura = isSearch(tree->arv,tree);

	return balanceada && alturas && procura;
}

/**
 *@brief			Função que devolve o número de nodos da árvore.
 *@param tree		Estrutura que contém a árvore.
 *@return 			Numero de nodos da árvore.
*/
long NUM_nodes(TREE t){
	return t->nnodes;
}

/**
 *@brief			Função que faz uma travessia inorder na árvore.
 *@param aux		Apontador para a arvore.
 *@param f_nodo		Função a aplicar a cada nodo.
 *@param data1		Apontador a passar como argumento à função a aplicar.
 *@param data2		Apontador a passar como argumento à função a aplicar.
 *@param begin		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param end		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param n			Número máximo de nodos a percorrer. (nullable)
*/
static void trans_inorder(AVL aux,void (*f_nodo)(void *,void *,void *, void *),void * data1, void * data2, void * begin, void * end, int * n){
	if (aux){
		if (begin == NULL || end == NULL) {
			trans_inorder(aux->esq,f_nodo,data1, data2, begin, end, n);
			f_nodo(aux->data,data1, data2, n);
			trans_inorder(aux->dir,f_nodo,data1, data2, begin, end, n);
		}
		else {
			int r1 = compare_MYDATE_AVL((MYDATE) begin, (MYDATE) aux->key );
			int r2 = compare_MYDATE_AVL((MYDATE) end, (MYDATE) aux->key);
			if(r1 >= 0 && r2 <= 0) {
				trans_inorder(aux->esq,f_nodo,data1, data2, begin, end, n);
				f_nodo(aux->data,data1, data2, n);
				trans_inorder(aux->dir,f_nodo,data1, data2, begin, end, n);
			}
			else if (r1 >= 0)
				trans_inorder(aux->esq,f_nodo,data1, data2, begin, end, n);
			else if (r2 <= 0)
				trans_inorder(aux->dir,f_nodo,data1, data2, begin, end, n);
		}
	}
}


/**
 *@brief			Função que faz uma travessia revinorder na árvore.
 *@param aux		Apontador para a arvore.
 *@param f_nodo		Função a aplicar a cada nodo.
 *@param data1		Apontador a passar como argumento à função a aplicar.
 *@param data2		Apontador a passar como argumento à função a aplicar.
 *@param begin		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param end		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param n			Número máximo de nodos a percorrer. (nullable)
*/
static void trans_revinorder(AVL aux,void (*f_nodo)(void *,void *,void *, void *),void * data1, void * data2, void * begin, void * end, int * n){
	if (aux){
		if (*n <= 0)
			return;
		if (begin == NULL || end == NULL) {
			trans_revinorder(aux->dir,f_nodo,data1, data2, begin, end, n);
			f_nodo(aux->data,data1, data2, n);
			trans_revinorder(aux->esq,f_nodo,data1, data2, begin, end, n);
		}
		else {
			int r1 = compare_MYDATE_AVL((MYDATE) begin, (MYDATE) aux->key );
			int r2 = compare_MYDATE_AVL((MYDATE) end, (MYDATE) aux->key);
			if(r1 >= 0 && r2 <= 0) {
				trans_revinorder(aux->dir,f_nodo,data1, data2, begin, end, n);
				f_nodo(aux->data,data1, data2, n);
				trans_revinorder(aux->esq,f_nodo,data1, data2, begin, end, n);
			}
			else if (r1 >= 0)
				trans_revinorder(aux->esq,f_nodo,data1, data2, begin, end, n);
			else if (r2 <= 0)
				trans_revinorder(aux->dir,f_nodo,data1, data2, begin, end, n);
		}
	}
}


/**
 *@brief			Função que faz uma travessia postorder na árvore.
 *@param aux		Apontador para a arvore.
 *@param f_nodo		Função a aplicar a cada nodo.
 *@param data1		Apontador a passar como argumento à função a aplicar.
 *@param data2		Apontador a passar como argumento à função a aplicar.
 *@param begin		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param end		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param n			Número máximo de nodos a percorrer. (nullable)
*/
static void trans_posorder(AVL aux,void (*f_nodo)(void *,void *,void *, void *),void * data1, void * data2, void * begin, void * end, int * n){
	if (aux){
		if (*n <= 0)
			return;
		if (begin == NULL || end == NULL) {
			trans_posorder(aux->esq,f_nodo,data1, data2, begin, end, n);
			trans_posorder(aux->dir,f_nodo,data1, data2, begin, end, n);
			f_nodo(aux->data,data1, data2, n);
		}
		else {
			int r1 = compare_MYDATE_AVL((MYDATE) begin, (MYDATE) aux->key );
			int r2 = compare_MYDATE_AVL((MYDATE) end, (MYDATE) aux->key);
			if(r1 >= 0 && r2 <= 0) {
				trans_posorder(aux->esq,f_nodo,data1, data2, begin, end, n);
				trans_posorder(aux->dir,f_nodo,data1, data2, begin, end, n);
				f_nodo(aux->data,data1, data2, n);
			}
			else if (r1 >= 0)
				trans_posorder(aux->esq,f_nodo,data1, data2, begin, end, n);
			else if (r2 <= 0)
				trans_posorder(aux->dir,f_nodo,data1, data2, begin, end, n);
		}
	}
}

/**
 *@brief			Função que faz uma travessia preorder na árvore.
 *@param aux		Apontador para a arvore.
 *@param f_nodo		Função a aplicar a cada nodo.
 *@param data1		Apontador a passar como argumento à função a aplicar.
 *@param data2		Apontador a passar como argumento à função a aplicar.
 *@param begin		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param end		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param n			Número máximo de nodos a percorrer. (nullable)
*/
static void trans_preorder(AVL aux,void (*f_nodo)(void *,void *,void *, void *),void * data1, void * data2, void * begin, void * end, int * n){
	if (aux){
		if (*n <= 0)
			return;
		if (begin == NULL || end == NULL) {
			f_nodo(aux->data,data1, data2, n);
			trans_posorder(aux->esq,f_nodo,data1, data2, begin, end, n);
			trans_posorder(aux->dir,f_nodo,data1, data2, begin, end, n);
		}
		else {
			int r1 = compare_MYDATE_AVL((MYDATE) begin, (MYDATE) aux->key );
			int r2 = compare_MYDATE_AVL((MYDATE) end, (MYDATE) aux->key);
			if(r1 >= 0 && r2 <= 0) {
				f_nodo(aux->data,data1, data2, n);
				trans_preorder(aux->esq,f_nodo,data1, data2, begin, end, n);
				trans_preorder(aux->dir,f_nodo,data1, data2, begin, end, n);
			}
			else if (r1 >= 0)
				trans_preorder(aux->esq,f_nodo,data1, data2, begin, end, n);
			else if (r2 <= 0)
				trans_preorder(aux->dir,f_nodo,data1, data2, begin, end, n);
		}

	}
}



/**
 *@brief			Função que faz uma travessia na árvore com 4 argumentos.
 *@param aux		Apontador para a arvore.
 *@param f_nodo		Função a aplicar a cada nodo.
 *@param data1		Apontador a passar como argumento à função a aplicar.
 *@param data2		Apontador a passar como argumento à função a aplicar.
 *@param begin		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param end		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param n			Número máximo de nodos a percorrer. (nullable)
*/
static void trans_4_args(AVL aux,void (*f_nodo)(void *,void *,void *, void *),void * data1, void * data2, void * begin, void * end){
	if (aux){
		if (begin == NULL || end == NULL) {
			trans_4_args(aux->esq,f_nodo,data1, data2, begin, end);
			f_nodo(aux->data,data1, begin,end);
			trans_4_args(aux->dir,f_nodo,data1, data2, begin, end);
		}
		else {
			int r1 = compare_MYDATE_AVL((MYDATE) begin, (MYDATE) aux->key );
			int r2 = compare_MYDATE_AVL((MYDATE) end, (MYDATE) aux->key);
			if(r1 >= 0 && r2 <= 0) {
				trans_4_args(aux->esq,f_nodo,data1, data2, begin, end);
				f_nodo(aux->data,data1, begin,end);
				trans_4_args(aux->dir,f_nodo,data1, data2, begin, end);
			}
			else if (r1 >= 0)
				trans_4_args(aux->esq,f_nodo,data1, data2, begin, end);
			else if (r2 <= 0)
				trans_4_args(aux->dir,f_nodo,data1, data2, begin, end);
		}
	}
}


/**
 *@brief			Função que faz uma travessia na árvore.
 *@param e		Apontador para a arvore.
 *@param f_nodo		Função a aplicar a cada nodo.
 *@param data1		Apontador a passar como argumento à função a aplicar.
 *@param data2		Apontador a passar como argumento à função a aplicar.
 *@param begin		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param end		Data início do intervalo a que o nodo tem de pertencer. (nullable)
 *@param travessia	Tipo de travessia.
 *@param n			Número máximo de nodos a percorrer. (nullable)
*/
void trans_tree(TREE e,void (*f_nodo)(void *,void *,void *, void *),void * data1, void * data2, void * begin, void * end, int travessia, int n){
	if (!e)
		return;
	if (travessia == 1)
		trans_posorder(e->arv,f_nodo,data1, data2, begin, end, &n);
	else if (travessia == 2)
		trans_inorder(e->arv,f_nodo,data1, data2, begin, end, &n);
	else if (travessia == 3)
		trans_preorder(e->arv,f_nodo,data1, data2, begin, end, &n);
	else if (travessia == 4) {
		trans_revinorder(e->arv, f_nodo, data1, data2, begin, end, &n);
	}
	else if (travessia == 5){
		trans_4_args(e->arv, f_nodo, data1, data2, begin, end);
	}
}
