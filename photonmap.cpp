#include"photonmap.h"
#include<iostream>
#include<cmath>

const double INF = 1e8;

Nearestphotons::Nearestphotons() {
	max_photons = found = 0;
	got_heap = false;
	dist2 = NULL;
	photons = NULL;
}

Nearestphotons::~Nearestphotons() {
	delete[] dist2;
	delete[] photons;
}

Photonmap::Photonmap( int size ) {
	max_photons = size;
	stored_photons = 0;
	photons = new Photon[size + 1];
	box_min = Vector3( INF , INF , INF );
	box_max = Vector3( -INF , -INF , -INF );
}

Photonmap::~Photonmap() {
	delete[] photons;
}

void Photonmap::Store( Photon photon ) {
	if ( stored_photons >= max_photons ) return;
	photons[++stored_photons] = photon;
	box_min = Vector3( std::min( box_min.x , photon.pos.x ) , std::min( box_min.y , photon.pos.y ) , std::min( box_min.z , photon.pos.z ) );
	box_max = Vector3( std::max( box_max.x , photon.pos.x ) , std::max( box_max.y , photon.pos.y ) , std::max( box_max.z , photon.pos.z ) );
}

void Photonmap::Balance() {
	Photon* porg = new Photon[stored_photons + 1];

	for ( int i = 0 ; i <= stored_photons ; i++ )
		porg[i] = photons[i];
	
	BalanceSegment( porg , 1 , 1 , stored_photons );
	delete[] porg;
}

void Photonmap::BalanceSegment( Photon* porg , int index , int st , int en ) {
	if ( st == en ) {
		photons[index] = porg[st];
		return;
	}

	int med = 1;
	while ( 4 * med <= en - st + 1 ) med <<= 1;

	if ( 3 * med <= en - st + 1 ) med = med * 2 + st - 1;
		else med = en + 1 - med;
	
	int axis = 2;
	if ( box_max.x - box_min.x > box_max.y - box_min.y && box_max.x - box_min.x > box_max.z - box_min.z ) axis = 0; else
	if ( box_max.y - box_min.y > box_max.z - box_min.z ) axis = 1;

	MedianSplit( porg , st , en , med , axis );
	photons[index] = porg[med];
	photons[index].plane = axis;

	if ( st < med ) {
		double tmp = box_max.GetCoord( axis );
		box_max.GetCoord( axis ) = photons[index].pos.GetCoord( axis );
		BalanceSegment( porg , index * 2 , st , med - 1 );
		box_max.GetCoord( axis ) = tmp;
	}

	if ( med < en ) {
		double tmp = box_min.GetCoord( axis );
		box_min.GetCoord( axis ) = photons[index].pos.GetCoord( axis );
		BalanceSegment( porg , index * 2 + 1 , med + 1 , en );
		box_min.GetCoord( axis ) = tmp;
	}
}

void Photonmap::MedianSplit( Photon* porg , int st , int en , int med , int axis ) {
	int l = st , r = en;

	while ( l < r ) {
		double key = porg[r].pos.GetCoord( axis );
		int i = l - 1 , j = r;
		for ( ; ; ) {
			while ( porg[++i].pos.GetCoord( axis ) < key );
			while ( porg[--j].pos.GetCoord( axis ) > key && j > l );
			if ( i >= j ) break;
			std::swap( porg[i] , porg[j] );
		}

		std::swap( porg[i] , porg[r] );
		if ( i >= med ) r = i - 1;
		if ( i <= med ) l = i + 1;
	}
}

Color Photonmap::GetIrradiance( Vector3 O , Vector3 N , double max_dist , int n ) {
	Color ret;

	Nearestphotons np;
	np.pos = O;
	np.max_photons = n;
	np.dist2 = new double[n + 1];
	np.photons = new Photon*[n + 1];
	np.dist2[0] = max_dist * max_dist;

	LocatePhotons( &np , 1 );
	if ( np.found <= 8 ) return ret;

	for ( int i = 1 ; i <= np.found ; i++ ) {
		Vector3 dir = np.photons[i]->dir;
		if ( N.Dot( dir ) < 0 ) ret += np.photons[i]->power;
	}
	
	ret = ret * ( 4 / ( emit_photons * np.dist2[0] ) );
	return ret;
}

void Photonmap::LocatePhotons( Nearestphotons* np , int index ) {
	if ( index > stored_photons ) return;
	Photon *photon = &photons[index];

	if ( index * 2 <= stored_photons ) {
		double dist = np->pos.GetCoord( photon->plane ) - photon->pos.GetCoord( photon->plane );
		if ( dist < 0 ) {
			LocatePhotons( np , index * 2 );
			if ( dist * dist < np->dist2[0] ) LocatePhotons( np , index * 2 + 1 );
		} else {
			LocatePhotons( np , index * 2 + 1 );
			if ( dist * dist < np->dist2[0] ) LocatePhotons( np , index * 2 );
		}
	}

	double dist2 = photon->pos.Distance2( np->pos );
	if ( dist2 > np->dist2[0] ) return;

	if ( np->found < np->max_photons ) {
		np->found++;
		np->dist2[np->found] = dist2;
		np->photons[np->found] = photon;
	} else {
		if ( np->got_heap == false ) {
			for ( int i = np->found >> 1 ; i >= 1 ; i-- ) {
				int par = i;
				Photon* tmp_photon = np->photons[i];
				double tmp_dist2 = np->dist2[i];
				while ( ( par << 1 ) <= np->found ) {
					int j = par << 1;
					if ( j + 1 <= np->found && np->dist2[j] < np->dist2[j + 1] ) j++;
					if ( tmp_dist2 >= np->dist2[j] ) break;
					
					np->photons[par] = np->photons[j];
					np->dist2[par] = np->dist2[j];
					par = j;
				}
				np->photons[par] = tmp_photon;
				np->dist2[par] = tmp_dist2;
			}
			np->got_heap = true;
		}

		int par = 1;
		while ( ( par << 1 ) <= np->found ) {
			int j = par << 1;
			if ( j + 1 <= np->found && np->dist2[j] < np->dist2[j + 1] ) j++;
			if ( dist2 > np->dist2[j] ) break;

			np->photons[par] = np->photons[j];
			np->dist2[par] = np->dist2[j];
			par = j;
		}
		np->photons[par] = photon;
		np->dist2[par] = dist2;

		np->dist2[0] = np->dist2[1];
	}
}
