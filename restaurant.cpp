#include "restaurant.h"

/*
	Sets *ptr to the i'th restaurant. If this restaurant is already in the cache,
	it just copies it directly from the cache to *ptr. Otherwise, it fetches
	the block containing the i'th restaurant and stores it in the cache before
	setting *ptr to it.
*/

void getRestaurant(restaurant* ptr, int i, Sd2Card* card, RestCache* cache) {
	uint32_t block = REST_START_BLOCK + i/8;
	if (block != cache->cachedBlock) {
		while (!card->readBlock(block, (uint8_t*) cache->block)) {
			Serial.print("readblock failed, try again");
		}
		cache->cachedBlock = block;
	}
	*ptr = cache->block[i%8];
}


// Selection sort to sort the restaurants.
void quicksort(RestDist *start, RestDist *end) {
	RestDist *i = start;
	RestDist *j = end;
	RestDist *pi = start;

	if (i >= j){
		return;
	}

	while(true){
		//find a distance from right which smaller than pivot
		while (j > i && (*j).dist >= (*pi).dist ){
			j--;
		}
		//find a distance from left which bigger than pivot
		while (i < j && (*i).dist <= (*pi).dist){
			i++;
		}
		//if i, j meet
		if (i == j){
			break;
		}
		// Swaps the two restaurants
		RestDist tmp1 = *i;
		*i = *j;
		*j = tmp1;


	}
	// Swaps the two restaurants
  RestDist tmp2 = *pi;
	*pi = *i;
	*i = tmp2;

	//recursion
	//divide two parts and quicksort them respectively
	quicksort(start, i-1);
	quicksort(i+1, end);

}

// Computes the manhattan distance between two points (x1, y1) and (x2, y2).
int16_t manhattan(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	return abs(x1-x2) + abs(y1-y2);
}

/*
	Fetches all restaurants from the card, saves their RestDist information
	in restaurants[], and then sorts them based on their distance to the
	point on the map represented by the MapView.
*/

void getAndSortRestaurants(const MapView& mv, RestDist restaurants[], Sd2Card* card, RestCache* cache, int minrating, int size) {
	restaurant r;

	// first get all the restaurants and store their corresponding RestDist information.
	for (int i = 0; i < NUM_RESTAURANTS; ++i) {

		getRestaurant(&r, i, card, cache);
		//only add the certain rating of restaurants to array
		if(max(floor(r.rating+1)/2, 1) >= minrating){
			restaurants[size].index = i;
			restaurants[size].dist = manhattan(lat_to_y(r.lat), lon_to_x(r.lon),
																					mv.mapY + mv.cursorY, mv.mapX + mv.cursorX);
			//declare the size of whole array
			size += 1;
		}
	}

		//add terminated to show the length of array, which would be used in scrollingMenu
 	restaurants[size].dist = '\0';

	// Now sort them.
	quicksort(&restaurants[0], &restaurants[size -1]);
}
