typedef struct {
	// do not change this order, it's the one
	// we need for interleaved OpenGL arrays
	// (GL_T2F_N3F_V3F)
	float u, v;
	float nx, ny, nz;
	float px, py, pz;
} VertexT;

typedef struct VertexHashNodeStructT {
	struct VertexHashNodeStructT* next;
	long index;
	VertexT vertex;
} VertexHashNodeT;

typedef struct {
	VertexHashNodeT* hashTable[ HASH_TABLE_SIZE ];
	long count;
} VertexHashT;

VertexHashT* VertexHash_New();

long VertexHash_Add(
	VertexHashT* vhash, const VertexT* vertex );

VertexT* VertexHash_ToArray(
	VertexHashT* vhash,
	long* count );
	
void VertexHash_Dispose( VertexHashT* vhash );
