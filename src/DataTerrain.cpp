//===========================================================================
// DataTerrain.cpp
//
// CDataTerrain
//   terrain data.
//   Contains height information, roughness settings, and surface texture.
//===========================================================================


#include "stdafx.h"
#include "CSApp.h"
#include "DataTerrain.h"


/////////////////////////////////////////////////////////////////////////////
// Construction / Destruction

CDataTerrain::CDataTerrain()
{
	heights = upperNormals = lowerNormals = NULL;	/// UGLY!

	Clear();
}

CDataTerrain::~CDataTerrain()
{
	/// UGLY!
	delete[] heights;
	delete[] upperNormals;
	delete[] lowerNormals;
}

void CDataTerrain::Clear()
{
	/// UGLY!
	delete[] heights;
	delete[] upperNormals;
	delete[] lowerNormals;

	size = arraySize = 0;

	viewHeight = 0.0f;
}

void CDataTerrain::New()
{
	Clear();

	// Generate new seed
	seed = (unsigned)clock(); 
	seed = 10; /// 10 looks horid

	size = 1<<optionsMgr.GetTerrIters(); // 2^

	arraySize = size + 1;

	heights = new float[arraySize * arraySize];
	upperNormals = new float[ size*size*3 ];
	lowerNormals = new float[ size*size*3 ];

	MakeTerrain();
}


/////////////////////////////////////////////////////////////////////////////
// Gets

float*		CDataTerrain::GetHeights()		{	return heights;		}
int			CDataTerrain::GetArraySize()	{	return arraySize;	}
int			CDataTerrain::GetSize()			{	return size;		}
float		CDataTerrain::GetViewHeight()	{	return viewHeight;	}

float CDataTerrain::GetHeight( int i, int j )
{
	return heights[ i*arraySize + j ];
}

float* CDataTerrain::GetUpperNormal( int i, int j )
{
	return &upperNormals [ (size*j*3 + i*3) ];
}

float* CDataTerrain::GetLowerNormal( int i, int j )
{
	return &lowerNormals [ (size*j*3 + i*3) ];
}


/////////////////////////////////////////////////////////////////////////////
// Sets

void CDataTerrain::SetUpperNormal( int i, int j, float* n )
{
	upperNormals[ (size*j*3 + i*3 + 0) ] = n[0];
	upperNormals[ (size*j*3 + i*3 + 1) ] = n[1];
	upperNormals[ (size*j*3 + i*3 + 2) ] = n[2];
}

void CDataTerrain::SetLowerNormal( int i, int j, float* n )
{
	lowerNormals[ (size*j*3 + i*3 + 0) ] = n[0];
	lowerNormals[ (size*j*3 + i*3 + 1) ] = n[1];
	lowerNormals[ (size*j*3 + i*3 + 2) ] = n[2];
}

void CDataTerrain::IncViewHeight()///
{	viewHeight += 0.01f;	}
void CDataTerrain::DecViewHeight()///
{	viewHeight -= 0.01f;	}


/////////////////////////////////////////////////////////////////////////////
// Methods

void CDataTerrain::MakeTerrain()
{
	// Set seed so terrain is predictable
	srand( seed );

	float roughness = optionsMgr.GetTerrRoughness();

	int i;        // row index
	int j;        // col index
	float range;  // range of offset
	int midSize;  // used to keep track of current iteration
	BOOL findingOddPoints;

	for (i=0; i<arraySize; ++i)
	{
		for (j=0; j<arraySize; ++j)
		{
			heights[(i*arraySize) + j] = 0.0f;
		}
	}

	range = roughness;
	midSize = size / 2;

	while (midSize > 0)
	{
		/* FINDING * BY OFFSETTING AVERAGE OF X's
		    In this case size = 8, arraySize = 9, midSize = 2.
			X . . . X . . . X
			. . . . . . . . .
			. . * . . . * . .
			. . . . . . . . .
			X . . . X . . . X
			. . . . . . . . .
			. . * . . . * . .
			. . . . . . . . .
			X . . . X . . . X
		*/
		for (i=midSize; i<size; i+=midSize)
		{
			for (j=midSize; j<size; j+=midSize)
			{
				heights[(i*arraySize) + j] = 
					RandomOffset(range) + 
					AvgSquare(i, j, midSize);
			}
		}

		/* FINDING * BY OFFSETTING AVERAGE OF X's
		    In this case size = 8, arraySize = 9, midSize = 2.
			X . * . X . * . X
			. . . . . . . . .
			* . X . * . X . *
			. . . . . . . . .
			X . * . X . * . X
			. . . . . . . . .
			* . X . * . X . *
			. . . . . . . . .
			X . * . X . * . X
		*/
		findingOddPoints = FALSE;
		for (i=0; i<=size; i+=midSize)
		{
			findingOddPoints = !findingOddPoints;
			for (j=0; j<=size; j+=midSize*2)
			{
				// Offset initial j for odd points (1st row, 3rd row, ...)
				if (findingOddPoints && j==0)
					j += midSize;

				heights[ (i*arraySize) + j ] =
					RandomOffset(range) +
					AvgDiamond(i, j, midSize);
			}
		}

		midSize /= 2;
		range *= roughness;
	}

	CalculateNormals();
	CalculateViewHeight();
}

float CDataTerrain::AvgSquare(int i, int j, int midSize)
{
	float total = 
		heights[ ((i + midSize) * arraySize) + (j + midSize) ] +
		heights[ ((i + midSize) * arraySize) + (j - midSize) ] +
		heights[ ((i - midSize) * arraySize) + (j + midSize) ] +
		heights[ ((i - midSize) * arraySize) + (j - midSize) ];

	return total / 4;
}

float CDataTerrain::AvgDiamond(int i, int j, int midSize)
{
	float total;
	if (i == 0)
	total = heights[ (i*arraySize) + j-midSize ] +
			heights[ (i*arraySize) + j+midSize ] +
			heights[ ((size-midSize)*arraySize) + j ] +
			heights[ ((i+midSize)*arraySize) + j ];
	else if (i == arraySize-1)
	total = heights[ (i*arraySize) + j-midSize ] +
			heights[ (i*arraySize) + j+midSize ] +
			heights[ ((i-midSize)*arraySize) + j ] +
			heights[ ((0+midSize)*arraySize) + j ];
	else if (j == 0)
	total = heights[ ((i-midSize)*arraySize) + j ] +
			heights[ ((i+midSize)*arraySize) + j ] +
			heights[ (i*arraySize) + j+midSize ] +
			heights[ (i*arraySize) + size-midSize] ;
	else if (j == arraySize-1)
	total = heights[ ((i-midSize)*arraySize) + j ] +
			heights[ ((i+midSize)*arraySize) + j ] +
			heights[ (i*arraySize) + j-midSize ] +
			heights[ (i*arraySize) + 0+midSize ];
	else
	total = heights[ ((i-midSize)*arraySize) + j ] +
			heights[ ((i+midSize)*arraySize) + j ] +
			heights[ (i*arraySize) + j-midSize ] +
			heights[ (i*arraySize) + j+midSize ];
	return total / 4;
}

// Pick random offset preferrably negative (valleys)
float CDataTerrain::RandomOffset( float range )
{
	float ratio = (float)(rand()%10000)/10000;

	return ratio * (2 * range) - range;
}


void CDataTerrain::CalculateNormals()
{
	float vec1[3];
	float vec2[3];
	float normal[3];

	int iterations = optionsMgr.GetTerrIters();

//	inc = (float)pow(2, -iterations+1);

	for( int x=0; x<size; x++ )
	{
		for( int z=0; z<size; z++ )
		{
			// Upper triangle
			vec1[0] = (float) pow(2, -iterations+1);
			vec1[1] = GetHeight( x+1, z ) - GetHeight( x, z );
			vec1[2] = 0;
			vec2[0] = 0;
			vec2[1] = GetHeight( x, z+1 ) - GetHeight( x, z );
			vec2[2] = (float) pow(2, -iterations+1);

			CalculateNormal( vec1, vec2, normal );
			SetUpperNormal( x, z, normal );

			// Lower triangle
			vec1[0] = -(float) pow(2, -iterations+1);
			vec1[1] = GetHeight( x+1, z ) - GetHeight( x+1, z+1 );
			vec1[2] = 0;
			vec2[0] = 0;
			vec2[1] = GetHeight( x, z+1 ) - GetHeight( x+1, z+1 );
			vec2[2] = -(float) pow(2, -iterations+1);

			CalculateNormal( vec1, vec2, normal );
			SetLowerNormal( x, z, normal );
		}
	}
}

void CDataTerrain::CalculateNormal( float* vec1, float* vec2, float* normal )
{
	normal[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1];
	normal[1] = vec1[2]*vec2[0] - vec1[0]*vec2[2];
	normal[2] = vec1[0]*vec2[1] - vec1[1]*vec2[0];

	float normalLength = (float) sqrt (
		normal[0]*normal[0] +
		normal[1]*normal[1] +
		normal[2]*normal[2] );

	normal[0] /= normalLength;
	normal[1] /= normalLength;
	normal[2] /= normalLength;

	// Make sure we have upward normals
	if( normal[1] < 0 )
	{
		normal[0] = -normal[0];
		normal[1] = -normal[1];
		normal[2] = -normal[2];
	}
}

void CDataTerrain::CalculateViewHeight()
{
	// Get the index of the midpoint
	int middleIndex = size / 2;

	// Just set the viewer a little above the midpoint
	viewHeight = GetHeight( middleIndex, middleIndex ) + 0.05f;
}



