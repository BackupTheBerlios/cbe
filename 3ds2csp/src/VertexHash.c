#include "smooth.h"
#include "VertexHash.h"

static long
VertexHash( const VertexT* vtx )
{
	double h;
	
	h = 3*ABSQUANT(vtx->px);
	h += 5*ABSQUANT(vtx->py);
	h += 7*ABSQUANT(vtx->pz);

	h += 9*ABSQUANT(vtx->nx);
	h += 11*ABSQUANT(vtx->ny);
	h += 23*ABSQUANT(vtx->nz);

	h += 17*ABSQUANT(vtx->u);
	h += 19*ABSQUANT(vtx->v);
	
	return ( labs( h * HASH_TABLE_SIZE ) ) % HASH_TABLE_SIZE;
}

static int
IsSameVertex(
	const VertexT* va, const VertexT* vb )
{
	const float peps = 0.0001;
	const float neps = 0.01;
	const float uveps = 0.01;

	if( fabs( va->px - vb->px ) < peps &&
		fabs( va->py - vb->py ) < peps &&
		fabs( va->pz - vb->pz ) < peps &&
		fabs( va->nx - vb->nx ) < neps &&
		fabs( va->ny - vb->ny ) < neps &&
		fabs( va->nz - vb->nz ) < neps &&
		fabs( va->u - vb->u ) < uveps &&
		fabs( va->v - vb->v ) < uveps )
		return 1;
		
	return 0;
}

VertexHashT* VertexHash_New()
{
	VertexHashT* node = (VertexHashT*)malloc(
		sizeof( VertexHashT ) );
	memset( node->hashTable, 0,
		HASH_TABLE_SIZE * sizeof( VertexHashNodeT* ) );
	node->count = 0;
	return node;
}

long VertexHash_Add(
	VertexHashT* vhash, const VertexT* vertex )
{
	long h = VertexHash( vertex );
	VertexHashNodeT** link;
	VertexHashNodeT* node;
	
	link = &vhash->hashTable[ h ];
	node = *link;
	
	while( node )
	{
		if( IsSameVertex( &node->vertex, vertex ) )
			return node->index;
			
		node = node->next;
	}
	
	node = (VertexHashNodeT*)malloc(
		sizeof( VertexHashNodeT ) );
	node->next = *link;
	node->index = vhash->count++;
	node->vertex = *vertex;
	
	*link = node;
	
	return node->index;
}

VertexT* VertexHash_ToArray(
	VertexHashT* vhash,
	long* count )
{
	long i;
	long cnt = vhash->count;
	VertexT* array;
	VertexHashNodeT* node;

	*count = cnt;
	array = (VertexT*)malloc( sizeof( VertexT ) * cnt );
	
	for( i = 0; i < HASH_TABLE_SIZE; i++ )
	{
		node = vhash->hashTable[ i ];
		while( node )
		{
			array[ node->index ] = node->vertex;
			node = node->next;
		}
	}
	
	return array;
}

void
VertexHash_Dispose( VertexHashT* vhash )
{
	int i;
	VertexHashNodeT* node;
	VertexHashNodeT* next;
	
	for( i = 0; i < HASH_TABLE_SIZE; i++ )
	{
		node = vhash->hashTable[ i ];
		while( node )
		{
			next = node->next;
			free( node );
			node = next;
		}
	}

	free( vhash );
}
