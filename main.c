/* COP 3502C PA1
This program is written by: Cristoffer Bohorquez*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ====================================================================================
// PART 1: GLOBAL CONSTANT - Status array for cats
// ====================================================================================
// This array holds the 3 possible statuses a cat can have
// Cats will store a number (0, 1, or 2) that indexes into this array
const char STATUS_CAT[3][20] = {"ADOPTED", "PENDING", "AVAILABLE"};


// ====================================================================================
// PART 2: STRUCT DEFINITIONS (Given - Don't modify these!)
// ====================================================================================

// Represents a single cat
typedef struct Cat {
    char *name;        // dynamically allocated - we malloc space for the name
    int age;           // just a regular int
    float weight;      // just a regular float
    char *breed;       // IMPORTANT: This POINTS to dictionary, don't malloc for this!
    int status;        // 0=ADOPTED, 1=PENDING, 2=AVAILABLE (index into STATUS_CAT)
} Cat;

// Represents a kennel (a location that holds cats)
typedef struct Kennel {
    char *location;    // dynamically allocated location name
    int occupancy;     // how many cats are currently here
    Cat **cats;        // array of POINTERS to cats (double pointer!)
    int maxCapacity;   // maximum cats this kennel can hold
} Kennel;

// Represents the entire cat store
typedef struct CatStore {
    int **capacities;  // 2D array: capacities[kennel][breed] = max cats of that breed
    int numKennels;    // how many kennels we have
    Kennel *kennels;   // array of kennels
} CatStore;


// ====================================================================================
// PART 3: FUNCTION PROTOTYPES - These are the functions you MUST implement
// ====================================================================================

// --- Reading and Setup Functions ---
char **readBreeds(int *count);
char *getCharPtrByBreed(char **dictionary, char *breedName, int breedCount);
CatStore *createStore(int kennelCount, int breedCount, char **dictionary);
Kennel *createKennels(int **constraints, int kennelCount, int breedCount, char **dictionary);
Cat **createCats(char **dictionary, int breedCount, int count);
Cat *createSingleCat(char **dictionary, int breedCount);

// --- Query Helper Functions ---
int canMoveTo(CatStore *s, char *location, char *breed, char **dictionary, int breedCount);
Kennel *getKennelByCat(CatStore *s, Cat *cat);
int getCatPosi(Kennel *home, Cat *cat);
Cat *getCatByName(CatStore *s, char *catName);
void removeCatFromKennel(Kennel *k, Cat *cat);

// --- Query Processing ---
void runQueries(CatStore *s, char **dictionary, int breedCount, int numQueries);

// --- Memory Cleanup Functions ---
void freeBreeds(char **dictionary, int breedCount);
void freeStore(int count, CatStore *store);


// ====================================================================================
// PART 4: MAIN FUNCTION - The flow of the program
// ====================================================================================
int main() {
    
    // STEP 1: Read the breeds dictionary
    int breedCount = 0;
    char **dictionary = readBreeds(&breedCount);
    
    // STEP 2: Read the number of kennels
    int kennelCount;
    scanf("%d", &kennelCount);
    
    // STEP 3: Create the store (reads constraints, kennels, and cats)
    CatStore *store = createStore(kennelCount, breedCount, dictionary);
    
    // STEP 4: Read number of queries
    int numQueries;
    scanf("%d", &numQueries);
    
    // STEP 5: Process all queries
    runQueries(store, dictionary, breedCount, numQueries);
    
    // STEP 6: Clean up all memory
    freeStore(kennelCount, store);
    freeBreeds(dictionary, breedCount);
    
    return 0;
}


// ====================================================================================
// PART 5: IMPLEMENTATION OF FUNCTIONS
// ====================================================================================

// ---------------------
// Function: readBreeds
// Purpose: Reads breed names from input and stores them in a dynamically allocated array
// Returns: Array of strings (char **) containing breed names
// ---------------------
char **readBreeds(int *count) {
    // Step 1: Store the number of breeds in count
    scanf("%d", count);
    
    // Step 2: Getting the value of count and making the array save the correct storage with sizeof 
    char **dictionary = malloc((*count) * sizeof(char *));
    
    // Step 3: Read each breed
    char temp[100];  // Temporary buffer
    
    for (int i = 0; i < *count; i++) {
        scanf("%s", temp);
        
        // Allocate exact space for this breed
        dictionary[i] = malloc((strlen(temp) + 1) * sizeof(char));
        
        // Copy the breed name
        strcpy(dictionary[i], temp);
    }
    
    // Step 4: Return the dictionary
    return dictionary;
}



// ---------------------
// Function: getCharPtrByBreed
// Purpose: Finds a breed in the dictionary and returns pointer to it
// Returns: Pointer to the breed string in dictionary, or NULL if not found
// ---------------------
char *getCharPtrByBreed(char **dictionary, char *breedName, int breedCount) {
    // TODO:
    // Loop through dictionary and use strcmp to find matching breed

    for (int i = 0; i < breedCount; i++)
    {
        if (strcmp(dictionary[i],breedName) == 0)
        {
            return dictionary[i];
        }
        


    }
        //If we dont find the pointer
    return NULL; 
}


// ---------------------
// Function: createStore
// Purpose: Creates and initializes the entire CatStore
// Returns: Pointer to allocated CatStore
// ---------------------
CatStore *createStore(int kennelCount, int breedCount, char **dictionary) {
    // Allocate space for CatStore
    CatStore *store = malloc(sizeof(CatStore));
    
    // Allocate the 2D capacities array (rows)
    store->capacities = malloc(kennelCount * sizeof(int *));
    
    // Allocate each row (columns for each kennel)
    for (int i = 0; i < kennelCount; i++) {
        store->capacities[i] = malloc(breedCount * sizeof(int));
    }
    
    // Read capacity constraints for each kennel and breed
    for (int i = 0; i < kennelCount; i++) {
        for (int j = 0; j < breedCount; j++) {
            scanf("%d", &store->capacities[i][j]);
        }
    }
    
    // Create all kennels using createKennels
    store->kennels = createKennels(store->capacities, kennelCount, breedCount, dictionary);
    
    // Set the number of kennels
    store->numKennels = kennelCount;
    
    return store;
}


// ---------------------
// Function: createKennels
// Purpose: Creates array of kennels with their cats
// Returns: Array of Kennel structs
// ---------------------
Kennel *createKennels(int **constraints, int kennelCount, int breedCount, char **dictionary) {
    // Allocate array of Kennel structs
    Kennel *kennels = malloc(kennelCount * sizeof(Kennel));
    
    // For each kennel
    for (int i = 0; i < kennelCount; i++) {
        // Read location name (allocate dynamically)
        char temp[100];
        scanf("%s", temp);
        kennels[i].location = malloc((strlen(temp) + 1) * sizeof(char));
        strcpy(kennels[i].location, temp);
        
        // Read number of cats
        int numCats;
        scanf("%d", &numCats);
        
        // Create cats using createCats
        kennels[i].cats = createCats(dictionary, breedCount, numCats);
        
        // Set occupancy
        kennels[i].occupancy = numCats;
        
        // Calculate maxCapacity from constraints
        kennels[i].maxCapacity = 0;
        for (int j = 0; j < breedCount; j++) {
            kennels[i].maxCapacity += constraints[i][j];
        }
    }
    
    return kennels;  
}


// ---------------------
// Function: createCats
// Purpose: Creates an array of Cat pointers
// Returns: Array of Cat pointers (Cat **)
// ---------------------
Cat **createCats(char **dictionary, int breedCount, int count) {
    // TODO:
    // 1. Allocate array of Cat pointers

    Cat **cats = malloc(count * sizeof(Cat *));

    // 2. Loop to create each cat

    for (int i = 0; i < count; i++)
    {
        // 3. Create one cat and store it
            cats[i] = createSingleCat(dictionary, breedCount);

    }

    
    return cats; 
}


// ---------------------
// Function: createSingleCat
// Purpose: Creates a single cat by reading its info
// Returns: Pointer to allocated Cat
// ---------------------
Cat *createSingleCat(char **dictionary, int breedCount) {
    // TODO:
    // 1. Allocate space for one Cat struct


    Cat *cat = malloc(sizeof(Cat));

    // 2. Read the name using temp buffer

    char temp [100];
    scanf("%s", temp);
    cat -> name = malloc((strlen(temp)+1)*sizeof(char));
    strcpy(cat->name, temp);

    // 3. Read age and weight
    scanf("%d", &cat -> age);
    scanf("%f", &cat -> weight);

    // 4. Read breed name, find it in dictionary using getCharPtrByBreed
    char breedTemp[100];
    scanf("%s", breedTemp);
    cat->breed = getCharPtrByBreed(dictionary, breedTemp, breedCount);

    // 5. Set status to 2 (AVAILABLE)

    cat->status = 2;
    
    return cat;  
}


// ---------------------
// Function: canMoveTo
// Purpose: Checks if a cat can be moved to a specific kennel
// Returns: 1 if can move, 0 if cannot
// ---------------------
int canMoveTo(CatStore *s, char *location, char *breed, char **dictionary, int breedCount) {
    // Step 1: Find the kennel with this location
    Kennel *targetKennel = NULL;
    
    // Loop through all kennels to find the one with matching location
    for (int i = 0; i < s->numKennels; i++) {
        // s->kennels[i].location is the location name of the i-th kennel
        // strcmp returns 0 when strings match, so we check == 0
        if (strcmp(location, s->kennels[i].location) == 0) {
            // Found the kennel! Store its address
            targetKennel = &s->kennels[i];
            break;
        }
    }
    
    // Step 2: Find the breed index in dictionary
    int breedIndex = -1;
    
    // Loop through the breed dictionary
    for (int i = 0; i < breedCount; i++) {
        // Compare the breed name with each dictionary entry
        if (strcmp(breed, dictionary[i]) == 0) {
            // Found the breed! Store its index
            breedIndex = i;
            break;
        }
    }
    
    // Step 3: Count how many cats of this breed are already in the kennel
    int currentBreedCount = 0;
    
    // Loop through all cats in the target kennel
    for (int i = 0; i < targetKennel->occupancy; i++) {
        // targetKennel->cats[i] is the i-th cat pointer
        // targetKennel->cats[i]->breed is that cat's breed string
        if (strcmp(targetKennel->cats[i]->breed, breed) == 0) {
            // This cat matches the breed we're checking
            currentBreedCount++;
        }
    }
    
    // Step 4: Check if adding one more cat would exceed the overall maxCapacity
    // targetKennel->occupancy is current number of cats
    // targetKennel->maxCapacity is the maximum total cats allowed
    if (targetKennel->occupancy + 1 > targetKennel->maxCapacity) {
        // Would exceed total capacity - cannot move
        return 0;
    }
    
    // Step 5: Find the kennel's index in the store's kennels array
    int kennelIndex = -1;
    
    // Loop through kennels to find which index matches our targetKennel
    for (int i = 0; i < s->numKennels; i++) {
        // &s->kennels[i] is the address of the i-th kennel
        // Compare with targetKennel pointer
        if (&s->kennels[i] == targetKennel) {
            kennelIndex = i;
            break;
        }
    }
    
    // Step 6: Check if adding one more would exceed the breed-specific capacity
    // s->capacities[kennelIndex][breedIndex] is the max cats of this breed for this kennel
    if (currentBreedCount + 1 > s->capacities[kennelIndex][breedIndex]) {
        // Would exceed breed-specific capacity - cannot move
        return 0;
    }
    
    // All checks passed - cat can be moved to this kennel
    return 1;
}


// ---------------------
// Function: getKennelByCat
// Purpose: Finds which kennel contains a specific cat
// Returns: Pointer to the kennel, or NULL if not found
// ---------------------
Kennel *getKennelByCat(CatStore *s, Cat *cat) {
    // Loop through each kennel in the store
    for (int i = 0; i < s->numKennels; i++) {
        // s->kennels[i] accesses the i-th kennel in the kennels array
        // s->kennels[i].occupancy gets how many cats are in this kennel
        
        // Loop through each cat in the current kennel
        for (int j = 0; j < s->kennels[i].occupancy; j++) {
            // s->kennels[i].cats[j] is the j-th cat pointer in kennel i
            
            // Compare POINTERS, not names (use ==, not strcmp)
            // We're checking if the cat we're looking for is the same cat object
            if (cat == s->kennels[i].cats[j]) {
                // Found the cat! Return address of this kennel
                // &s->kennels[i] gets the address of the i-th kennel
                return &s->kennels[i];
            }
        }
    }
    
    // Searched all kennels - cat not found in any kennel
    return NULL;
}


// ---------------------
// Function: getCatPosi
// Purpose: Finds the index of a cat in a kennel's cat array
// Returns: Index of cat, or -1 if not found
// ---------------------
int getCatPosi(Kennel *home, Cat *cat) {
    // Loop through all cats in this kennel
    // home->occupancy is how many cats are in the kennel
    for (int i = 0; i < home->occupancy; i++) {
        // home->cats is the array of cat pointers
        // home->cats[i] is the i-th cat pointer in the array
        
        // Compare POINTERS to see if this is the cat we're looking for
        if (cat == home->cats[i]) {
            // Found it! Return the index position
            return i;
        }
    }
    
    // Cat not found in this kennel - return -1 to indicate failure
    return -1;
}


// ---------------------
// Function: getCatByName
// Purpose: Finds a cat by name in the entire store
// Returns: Pointer to the cat, or NULL if not found
// ---------------------
Cat *getCatByName(CatStore *s, char *catName) {
    // Loop through each kennel in the store
    for (int i = 0; i < s->numKennels; i++) {
        // s->kennels[i] accesses the i-th kennel in the kennels array
        // s->kennels[i].occupancy gets how many cats are in this kennel
        
        // Loop through each cat in the current kennel
        for (int j = 0; j < s->kennels[i].occupancy; j++) {
            // s->kennels[i].cats is the array of cat pointers for kennel i
            // s->kennels[i].cats[j] is the j-th cat pointer in that array
            // s->kennels[i].cats[j]->name accesses the name field of that cat
            
            // Compare the cat's name with the name we're looking for
            if (strcmp(catName, s->kennels[i].cats[j]->name) == 0) {
                // Found a match! Return pointer to this cat
                return s->kennels[i].cats[j];
            }
        }
    }
    
    // Searched all kennels and all cats - cat not found
    return NULL;
}


// ---------------------
// Function: removeCatFromKennel
// Purpose: Removes a cat from a kennel and resizes the array
// Returns: Nothing (void)
// ---------------------
void removeCatFromKennel(Kennel *k, Cat *cat) {
    // TODO:
    // 1. Find position of cat in array

        int position = getCatPosi(k, cat);

    // 2. Shift all cats after it down one position

        if (position == -1)
        {
            return;
        }

        for (int i = position; i < k->occupancy - 1  ; i++)
        {
            k->cats[i] = k->cats[i+1];
        }
        
        
    // 3. Decrease occupancy
        k->occupancy--;


    // 4. Realloc the cats array to be smaller

     k->cats = realloc(k->cats, k->occupancy * sizeof(Cat *));
    // NOTE: Don't free the cat itself!
    
}


// ---------------------
// Function: runQueries
// Purpose: Processes all query commands
// Returns: Nothing (void)
// ---------------------
void runQueries(CatStore *s, char **dictionary, int breedCount, int numQueries) {
    
    // Loop through all queries
    for (int i = 0; i < numQueries; i++) {
        
        int queryType;
        // Read the query type (1, 2, or 3)
        scanf("%d", &queryType);
        
        // Query Type 1: Print all cats of a specific breed
        if (queryType == 1) {
            
            // Read the breed name
            char breed[100];
            scanf("%s", breed);
            
            // Track if we found any cats
            int found = 0;
            
            // Loop through all kennels
            for (int j = 0; j < s->numKennels; j++) {
                
                // Loop through all cats in this kennel
                for (int k = 0; k < s->kennels[j].occupancy; k++) {
                    
                    // Check if this cat's breed matches
                    if (strcmp(s->kennels[j].cats[k]->breed, breed) == 0) {
                        
                        // Print cat info: name, weight, age, location, status
                        printf("%s %.2f %d %s %s\n",
                               s->kennels[j].cats[k]->name,
                               s->kennels[j].cats[k]->weight,
                               s->kennels[j].cats[k]->age,
                               s->kennels[j].location,
                               STATUS_CAT[s->kennels[j].cats[k]->status]);
                        
                        found = 1;
                    }
                }
            }
            
            // If no cats found, print message
            if (found == 0) {
                printf("No cat with breed %s\n", breed);
            }
        }
        
        // Query Type 2: Update cat status
        else if (queryType == 2) {
            
            int newStatus;
            char catName[100];
            
            // Read new status and cat name
            scanf("%d", &newStatus);
            scanf("%s", catName);
            
            // Find the cat by name
            Cat *cat = getCatByName(s, catName);
            
            // Update the cat's status
            cat->status = newStatus;
            
            // Print status update message
            printf("%s is now %s!\n", cat->name, STATUS_CAT[newStatus]);
            
            // If adopted, remove from kennel and free memory
            if (newStatus == 0) {
                
                // Find which kennel has this cat
                Kennel *home = getKennelByCat(s, cat);
                
                // Remove cat from kennel
                removeCatFromKennel(home, cat);
                
                // Free the cat's memory
                free(cat->name);
                free(cat);
            }
        }
        
        // Query Type 3: Move cat between kennels
        else if (queryType == 3) {
            
            char catName[100];
            char newLocation[100];
            
            // Read cat name and destination location
            scanf("%s", catName);
            scanf("%s", newLocation);
            
            // Find the cat
            Cat *cat = getCatByName(s, catName);
            
            // Check if cat can move to the new location
            if (canMoveTo(s, newLocation, cat->breed, dictionary, breedCount)) {
                
                // Can move!
                // Find cat's current kennel
                Kennel *oldKennel = getKennelByCat(s, cat);
                
                // Find the destination kennel
                Kennel *newKennel = NULL;
                for (int j = 0; j < s->numKennels; j++) {
                    if (strcmp(s->kennels[j].location, newLocation) == 0) {
                        newKennel = &s->kennels[j];
                        break;
                    }
                }
                
                // Remove from old kennel
                removeCatFromKennel(oldKennel, cat);
                
                // Resize new kennel's cats array
                newKennel->cats = realloc(newKennel->cats, (newKennel->occupancy + 1) * sizeof(Cat *));
                
                // Add cat to new kennel
                newKennel->cats[newKennel->occupancy] = cat;
                newKennel->occupancy++;
                
                // Print success message
                printf("%s moved successfully to %s\n", cat->name, newLocation);
                
            } else {
                // Cannot move - print error message
                printf("%s cannot take a %s cat!\n", newLocation, cat->breed);
            }
        }
    }
}


// ---------------------
// Function: freeBreeds
// Purpose: Frees all memory associated with the dictionary
// Returns: Nothing (void)
// ---------------------
void freeBreeds(char **dictionary, int breedCount) {
    // TODO:
    // 1. Free each breed string
        for (int i = 0; i < breedCount; i++)
        {
            free(dictionary[i]);
        }
        
    // 2. Free the array of pointers
    free(dictionary);
}


// ---------------------
// Function: freeStore
// Purpose: Frees all memory in the store
// Returns: Nothing (void)
// ---------------------
void freeStore(int count, CatStore *store) {
    
    // Free all cats in all kennels
    for (int i = 0; i < count; i++) {
        
        // Free each cat in this kennel
        for (int j = 0; j < store->kennels[i].occupancy; j++) {
            
            // Free cat's name
            free(store->kennels[i].cats[j]->name);
            
            // Free the cat itself
            free(store->kennels[i].cats[j]);
        }
        
        // Free the kennel's cats array
        free(store->kennels[i].cats);
        
        // Free the kennel's location
        free(store->kennels[i].location);
    }
    
    // Free the kennels array
    free(store->kennels);
    
    // Free the 2D capacities array
    for (int i = 0; i < count; i++) {
        // Free each row
        free(store->capacities[i]);
    }
    
    // Free the array of row pointers
    free(store->capacities);
    
    // Free the store itself
    free(store);
}

