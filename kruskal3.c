#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//estrutura do grafo
typedef struct aresta {
  int u;  //vertice de inicio
  int v; //vertice destino
  float custo;
  struct aresta *prox;
} aresta;

//estrutura de dados para os conjuntos disjuntos
typedef struct conj_disjunto {
  struct conj_disjunto	*p;	
  int	r;	 
} conj_disjunto;

conj_disjunto **cd;	/* conjunto disjunto para cada n um vertices */

//estrutura do conjunto de arestas
typedef struct conj_arestas {
  aresta 	*a;
  struct conj_arestas 	*prox;
} conj_arestas;

aresta **adj;
aresta **arestas;
int num_v, num_a, aresta_atual;		//numero de vertices e arestas

void cria_aresta( int u, int v, float custo )
{  
  aresta *a = ( aresta *) malloc(sizeof(aresta));	
  if (!a)
  printf("Erro: null!");

  if (u<0 || u>=num_v || v<0 || v>=num_v) 
  	printf("\nErro: %s\n Numero de vertices errado!");
  if ( aresta_atual >= num_a ) 
  	printf("\nErro: %s\n Muitas arestas");

  a->u = u;
  a->v = v;
  a->custo = custo;
  a->prox = adj[u];

  adj[u] = a;

  arestas[aresta_atual++] = a;
}

aresta *encontra_aresta( int u, int v )
{
  aresta *a;

  for ( a = adj[u]; a; a = a->prox )
    if ( a->v == v )
      return a;

  return NULL;
}

void insere_aresta(int u, int v, float custo)
{ 
  aresta *a;

  if (u<0 || u>=num_v || v<0 || v>=num_v) 
  	printf("\nERROR: %s\n Numero de vertices errado!");

  if ( u != v ) {
    a = encontra_aresta( u, v );
    if ( a ) {
      a->custo=((a->custo)>(custo)? (a->custo): (custo));
      a = encontra_aresta( v, u );
      assert( a );
      a->custo=((a->custo)>(custo)? (a->custo) : (custo));
    } else {
      cria_aresta( u, v, custo );
      cria_aresta( v, u, custo );
    }
  }
}

void inicia_grafo()
{ 
  adj   = calloc( num_v, sizeof(aresta *) );	     
  
  if (!adj)
  printf("Erro: null");

  arestas=(aresta*) malloc ((num_a)*sizeof(aresta));
  
  if (!arestas)
 printf("Erro: null");
  aresta_atual = 0;
}

void carrega_grafo( char* nome_arquivo )
{
  int u, v;
  float custo;

  FILE *arq_entrada = fopen(nome_arquivo,"r");
  if (!arq_entrada) printf("\nERROR: %s\n Erro ao abrir o arquivo");

  fscanf(arq_entrada,"%d %d", &num_v, &num_a);
  num_a *= 2;				/* Since graph is undirected */
  printf("Lendo o arquivo %s com %d vertices e %d arestas \n", nome_arquivo, num_v, num_a);
  inicia_grafo(); 
  while (fscanf(arq_entrada,"%d %d %f",&u,&v,&custo) != EOF) 
    insere_aresta( u, v, custo);
}      

conj_disjunto *cria_conjunto()
{ conj_disjunto *x = ( conj_disjunto *) malloc(sizeof(conj_disjunto));
    if (!x)
    	printf("Erro: null");
  x->p = x;
  x->r = 0;
  return x;
}

void inicia_conj_disjunto()
{ int i;

  cd=(conj_disjunto*)malloc((num_v)*sizeof(conj_disjunto));
   if (!cd)
 	printf("Erro: null");
  for (i=0; i<num_v; i++) 
  	cd[i] = cria_conjunto();
}

static void liga( conj_disjunto *x, conj_disjunto *y )
{ if ( x->r > y->r )
    y->p = x;
  else {
    x->p = y;
    if ( x->r == y->r )
      y->r++;
  }
}

conj_disjunto *encontra_conjunto( conj_disjunto *x )
{ if ( x != x->p )
    x->p = encontra_conjunto( x->p );
  return x->p;
}

void uniao( conj_disjunto *x, conj_disjunto *y )
{ liga( encontra_conjunto( x ), encontra_conjunto( y ));
}

conj_arestas *add_aresta_conjunto( aresta *a, conj_arestas *prox )
{
  conj_arestas *es = ( conj_arestas *)malloc(sizeof(conj_arestas));
  es->a 	= a;
  es->prox	= prox;
  return es;
}

int particiona( int p, int r )
{
  float x = arestas[p]->custo;
  int i = p-1;
  int j = r+1;

  while ( 1 ) {
    do {
      j--;
    } while ( arestas[j]->custo > x );

    do {
      i++;
    } while ( arestas[i]->custo < x );

    if ( i < j ) {
      register aresta *a = arestas[i];
      arestas[i] = arestas[j];
      arestas[j] = a;
    } else
      return j;      
  }
}

void ordena_arestas( int p, int r )
{
  if ( p < r ) {
    int q = particiona( p, r );

    ordena_arestas( p, q );
    ordena_arestas( q+1, r );
  }
}

//algoritml de kruskal

conj_arestas* alg_Kruskal()
{
  conj_arestas *A = NULL;
  int i;
  
  { ordena_arestas( 0, aresta_atual-1); }

  for ( i = 0; i < aresta_atual; i++ ) {
    aresta *a = arestas[i];
    int u = a->u;
    int v = a->v;
    
    if ( encontra_conjunto( cd[u] ) != encontra_conjunto( cd[v] ) ) {
      A = add_aresta_conjunto( a, A );	/*acrescenta a aresta ao conjunto A*/
      uniao( cd[u], cd[v] );
    }
  }
  
  return A;
}

void imprime_AEM( conj_arestas *A )
{
  float custo = 0.0;
  conj_arestas *h;
  
  printf( "Arvore Espalhada Minima: " );
  for ( h = A; h; h = h->prox ) {
    printf( "\n(%d, %d) ", h->a->u, h->a->v );    
    custo += h->a->custo;
  }

  printf("\nCusto da Arvore Espalhada Minima: %.1f\n", custo );
}

int main(int argc, char **argv)
{  
  conj_arestas *A; 

  carrega_grafo(argv[1]);
  
  inicia_conj_disjunto();
  A = alg_Kruskal();
  
  imprime_AEM( A );  
}
