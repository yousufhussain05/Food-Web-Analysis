/*-------------------------------------------
Program 2: Food Web Analysis with Dynamic Memory
Course: CS 211, Fall 2024, UIC
Author: Yousuf Hussain
-------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Struct to represent an organism and its prey in the food web
typedef struct Org_struct {
    char name[20]; //// Name of the organism
    int* prey; // Dynamic array of indices  
    int numPrey; // Number of prey this organism has
} Org;

// Adds a new organism to the food web
void addOrgToWeb(Org** ppWeb, int* pNumOrg, char* newOrgName) {
    if (*pNumOrg == 0) {
        *ppWeb = (Org*)malloc(sizeof(Org)); // Allocate memory for the first organism
    }

    else {
        Org *webCpy = (Org*)malloc(sizeof(Org)*(*(pNumOrg) + 1)); // Allocate memory for an additional organism
        
        // Copy existing organisms to the new array
        for (int i = 0; i < *pNumOrg; i++) {
            webCpy[i] = (*ppWeb)[i]; 
        }
        free(*ppWeb); // Free old memory and update pointer to new array
        *ppWeb = webCpy;
    }

    // Initialize the new organism
    strcpy((*ppWeb)[*pNumOrg].name, newOrgName);
    (*ppWeb)[*pNumOrg].prey = NULL; // No prey yet
    (*ppWeb)[*pNumOrg].numPrey = 0; // Initial prey count is 0
    (*pNumOrg)++; // // Increment number of organisms

    return;
}

// Adds a predator-prey relationship to the web
bool addRelationToWeb(Org* pWeb, int numOrg, int predInd, int preyInd) {
    // Ensure valid indices and prevent self-predation
    if (!(predInd >= 0 && preyInd >= 0 && predInd < numOrg &&  preyInd < numOrg && predInd != preyInd)) {
        printf( "Invalid predator and/or prey index. No relation added to the food web.");
        return false;
    }
    // Check if the relationship already exists
    for (int i = 0; i < pWeb[predInd].numPrey; i++) {
        if (((pWeb[predInd]).prey[i]) == preyInd) {
            printf("Duplicate predator/prey relation. No relation added to the food web.");
            return false;
        }
    }
 
    // Allocate memory to store the new relationship
    int *relation = (int*)malloc(sizeof(int)*((pWeb[predInd].numPrey) + 1));
    for (int i = 0; i < pWeb[predInd].numPrey; i++) {
        (relation)[i] = (pWeb[predInd].prey)[i]; // Copy old prey array
    }

    free(pWeb[predInd].prey); // Free the old prey array
    pWeb[predInd].prey = relation; // Assign the new prey array
    pWeb[predInd].prey[pWeb[predInd].numPrey] = preyInd; // Add the new prey
    (pWeb[predInd].numPrey)++; // Increment the prey count

    return true;    
}

// Frees the memory allocated for the food web
void freeWeb(Org* pWeb, int numOrg) {
    // Free each organism's prey list
    for (int i = 0; i < numOrg; i++) {
        free(pWeb[i].prey);
    }

    free(pWeb); // Free the main array holding organisms
}

// Removes an organism from the food web and updates relationships
bool removeOrgFromWeb(Org** ppWeb, int* pNumOrg, int index) {
    // Check if the index is valid
    if (index < 0 || index >= *pNumOrg) {
      printf("Invalid extinction index. No organism removed from the food web.");
      return false;
    }
    free((*ppWeb)[index].prey); // Free prey array of the organism to be removed

    // Case where only one organism remains
    if (*pNumOrg == 1) {
        free((*ppWeb)[index].prey);
        (*pNumOrg)--;
        free(*ppWeb); // Free the main organism array
        *ppWeb = NULL; // Set pointer to NULL as no organisms remain
        return true;
    }
    // Create a new array with one fewer organism
    Org* newWeb = (Org*)malloc((*pNumOrg - 1) * sizeof(Org));

    // Copy all organisms except the one to be removed
    for (int i = 0; i < index; i++) {
        newWeb[i] = (*ppWeb)[i];
    }
    for (int j = index + 1; j < *pNumOrg; j++) {
        newWeb[j - 1] = (*ppWeb)[j];
    }
    
    free(*ppWeb);
    *ppWeb = newWeb;
    (*pNumOrg)--;

    int* preyArr = NULL;
    // Update prey relationships to remove references to the extinct organism
    for (int i = 0; i < *pNumOrg; i++) {
        for (int j = 0; j < (*ppWeb)[i].numPrey; j++) {
            if ((*ppWeb)[i].prey[j] == index) { // If prey is the extinct organism
                if ((*ppWeb)[i].numPrey == 1) {
                    // Adjust the prey array
                    free((*ppWeb)[i].prey);
                    (*ppWeb)[i].prey = NULL;
                    (*ppWeb)[i].numPrey--;
                }
                else if ((*ppWeb)[i].numPrey != 1) {
                    preyArr = (int*)malloc(sizeof(int) * ((*ppWeb)[i].numPrey - 1));
                    for (int k = 0; k < j; k++) {
                        preyArr[k] = (*ppWeb)[i].prey[k];
                    }
                    for (int l = j + 1; l < (*ppWeb)[i].numPrey; l++) {
                        preyArr[l - 1] = (*ppWeb)[i].prey[l];
                    }
                    free((*ppWeb)[i].prey);
                    (*ppWeb)[i].prey = preyArr;
                    (*ppWeb)[i].numPrey = (*ppWeb)[i].numPrey - 1;
                }
            }
        }  
    }

    // Adjust prey indices after the removal
    for (int m = 0; m < *pNumOrg; m++) {
        for (int n = 0; n < (*ppWeb)[m].numPrey; n++) {
            if ((*ppWeb)[m].prey[n] > index) {
                (*ppWeb)[m].prey[n]--;
            }
        }
    } 

    return true;
   
}

// Prints the food web with organism names and their prey
void printWeb(Org* pWeb, int numOrg) {
    for (int i = 0; i < numOrg; i++) {
        printf("  (%d) %s", i, pWeb[i].name);
        if (pWeb[i].numPrey > 0) {
            printf(" eats ");
            for (int j = 0; j < pWeb[i].numPrey; j++) {
                if (j != (pWeb[i].numPrey - 1)) {
                    printf("%s, ", pWeb[pWeb[i].prey[j]].name);
                }
                else {
                    printf("%s", pWeb[pWeb[i].prey[j]].name);
                }
            }
        }
        printf("\n");
    }
}

// Finds and prints apex predators 
void apexPredators(Org *pWeb, int numOrg) {
    for (int i = 0; i < numOrg; i++) {
        bool found = false; // Check if the organism is preyed upon
        for (int j = 0; j < numOrg; j++) {
            for (int k = 0; k < pWeb[j].numPrey; k++) {
                if (pWeb[j].prey[k] == i) {
                    found = true; // Check if the organism is preyed upon
                    break;
                }
            }
            if (found) { // Stop searching if the organism is preyed upon
                break;
            }
        }
        if (!found) {
            printf("  %s\n", pWeb[i].name);
        }
    }
}

// Finds and prints all producers
void producers(Org *pWeb, int numOrg) {
    for (int i = 0; i < numOrg; i++) {
        if (pWeb[i].numPrey == 0) {
            printf("  %s\n", pWeb[i].name);
        }
    }
}

// Finds and prints the most flexible eaters
void mostFlexibleEaters(Org *pWeb, int numOrg) {
    int flexEaters = 0;
    for (int i = 0; i < numOrg; i++) {
        if (flexEaters < pWeb[i].numPrey) {
            flexEaters = pWeb[i].numPrey;
        }
    }
    for (int i = 0; i < numOrg; i++) {
        if (pWeb[i].numPrey == flexEaters) {
            printf("  %s\n", pWeb[i].name);
        }
    }
}

// Finds and prints the most tasty organism 
void tastiestFood(Org *pWeb, int numOrg) {
    int foodCount[numOrg]; // Array to count how many times each organism is prey
    for (int i = 0; i < numOrg; i++) {
        foodCount[i] = 0;
    }
    for (int i = 0; i < numOrg; i++) {
        for (int j = 0; j < pWeb[i].numPrey; j++) {
            foodCount[pWeb[i].prey[j]]++;
        }
    }
    int maxEaten = 0;
    for (int i = 0; i < numOrg; i++) {
        if (foodCount[i] > maxEaten) { // Find the organism that is eaten the most
            maxEaten = foodCount[i];
        }
    }
    for (int i = 0; i < numOrg; i++) {
        if (foodCount[i] == maxEaten) {
            printf("  %s\n", pWeb[i].name);
        }
    }
}

// Finds and prints the height of each organism in the food web
void heights(Org *pWeb, int numOrg) {
    int height[numOrg];
    for (int i = 0; i < numOrg; i++) {
        height[i] = 0;
    }

    bool go = true;

    // Loop until no more height updates are needed
    while(go) {
        go = false;
        for (int i = 0; i < numOrg; i++) {
            int heights = 0;
            for (int j = 0; j < pWeb[i].numPrey; j++) {
                int preyIdx = pWeb[i].prey[j];
                if (height[preyIdx] + 1 > heights) {
                    heights = height[preyIdx] + 1;
                }
            }
    
            if (height[i] < heights) {
                height[i] = heights;
                go = true;
            }
        }
    }

    for (int i = 0; i< numOrg; i++) {
        printf("  %s: %d\n", pWeb[i].name, height[i]);
    }
}

void voreType(Org *pWeb, int numOrg) {
    // Finds and prints producers
    printf("  Producers:\n");
    for (int i = 0; i < numOrg; i++) {
        if (pWeb[i].numPrey == 0) {
            printf("    %s\n", pWeb[i].name);
        }
    }
    // Finds and prints herbivores
    printf("  Herbivores:\n");
    for (int i = 0; i < numOrg; i++) {
        bool herbivore = true;
        for (int j = 0; j < pWeb[i].numPrey; j++) {
            if (pWeb[pWeb[i].prey[j]].numPrey != 0) {
                herbivore = false;
            }
        }
        if (herbivore && pWeb[i].numPrey > 0) {
            printf("    %s\n", pWeb[i].name);
        }
    }
    // Finds and prints omnivores
    printf("  Omnivores:\n");
    for (int i = 0; i < numOrg; i++) {
        bool producer = false;
        bool nonProducer = false;
        for (int j = 0; j < pWeb[i].numPrey; j++) {
            if (pWeb[pWeb[i].prey[j]].numPrey == 0) {
                producer = true;
            }
            else if (pWeb[pWeb[i].prey[j]].numPrey != 0) {
                nonProducer = true;
            }
        }
        if (producer && nonProducer) {
            printf("    %s\n", pWeb[i].name);
        }
    }
    // Finds and prints carnivores
    printf("  Carnivores:\n");
    for (int i = 0; i < numOrg; i++) {
        bool carnivore = true;
        for (int j = 0; j < pWeb[i].numPrey; j++) {
            if (pWeb[pWeb[i].prey[j]].numPrey == 0) {
                carnivore = false;
            }
        }
        if (carnivore && pWeb[i].numPrey > 0) {
            printf("    %s\n", pWeb[i].name);
        }
    }

}

void displayAll(Org* pWeb, int numOrg, bool modified) {
    if (modified) printf("UPDATED ");
    printf("Food Web Predators & Prey:\n");
    printWeb(pWeb,numOrg); 
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Apex Predators:\n");
    apexPredators(pWeb, numOrg);
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Producers:\n");
    producers(pWeb, numOrg);
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Most Flexible Eaters:\n");
    mostFlexibleEaters(pWeb, numOrg);
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Tastiest Food:\n");
    tastiestFood(pWeb, numOrg);
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Food Web Heights:\n");
    heights(pWeb, numOrg);
    printf("\n");

    if (modified) printf("UPDATED ");
    printf("Vore Types:\n");
    voreType(pWeb, numOrg);
    printf("\n");

}

bool setModes(int argc, char* argv[], bool* pBasicMode, bool* pDebugMode, bool* pQuietMode) {
    // Parse command-line arguments to set modes (basic, debug, quiet)
//     -b or -basic or -basicmode or -b*: basicMode = TRUE, which prevents the user from making any modifications to the initial food web. That is, the user can build a food web, for which the food web analysis will be completed and displayed, and then the program terminates, without any options provided to the user to modify the food web. This mode should be turned ON for any command-line argument string begins with -b as the first two characters. 
// -d or -debug or -debugmode or -d*: debugMode = TRUE, which prints out the full web as it is being built or modified, after each step of adding an organism to the web, adding a relationship to the web, or removing an organism from the web. This mode should be turned ON for any command-line argument string begins with -d as the first two characters.  
// -q or -quiet or -quietmode or -q*: quietMode = TRUE, which suppresses the printed prompt messages before each user-input scan. This is particularly helpful when running the program in a non-interactive fashion by redirecting user-input to a file that contains the full set of predefined user-inputs. This mode should be turned ON for any command-line argument string begins with -q as the first two characters. 

    for (int i = 1; i < argc; i++) {
        if ((argv[i][0] == '-') && (argv[i][1] == 'b')) {
            if (*pBasicMode == true) {
                return false;
            }
            *pBasicMode = true;
        
        }
        else if ((argv[i][0] == '-') && (argv[i][1] == 'd')) {
            if (*pDebugMode == true) {
                return false;
            }
            *pDebugMode = true;
        }
        else if ((argv[i][0] == '-') && (argv[i][1] == 'q')) {
            if (*pQuietMode == true) {
                return false;
            }
            *pQuietMode = true;
        }
        else {
            return false;
        }
    }

    return true; 

}


void printONorOFF(bool mode) {
    if (mode) {
        printf("ON\n");
    } else {
        printf("OFF\n");
    }
}


int main(int argc, char* argv[]) {  

    bool basicMode = false; // When set to true, only build the intial web, but cannot change it
    bool debugMode = false; // When set to true, print out full web as it is built & modified at each step
    bool quietMode = false; // When set to true, suppress printed user-input prompt messages 

    // Goes through the command-line arguments to change basicMode, debugMode, and quietMode
    // If valid command-line arguments are "-b", "-d", and "-q" (and can only appear once);
    // ALSO, allow full-word command-line arguments "-basic", "-debug", and "-quiet" by simply 
    if (!setModes(argc, argv, &basicMode, &debugMode, &quietMode)) {
        printf("Invalid command-line argument. Terminating program...\n");
        return 1;
    }

    // After parsing the command-line arguments the program 
    // Displays the status of all the modes
    // printONorOFF checks if each mode is ON or OFF
    printf("Program Settings:\n");
    printf("  basic mode = ");
    printONorOFF(basicMode);
    printf("  debug mode = ");
    printONorOFF(debugMode);
    printf("  quiet mode = ");
    printONorOFF(quietMode);
    printf("\n");

    
    
    int numOrgs = 0; // numOrgs tracks the # of organisms in the web
    printf("Welcome to the Food Web Application\n\n");
    printf("--------------------------------\n\n");

    Org* pWeb = NULL; // Pointer to a dynamic arr of organisms (type Org).
    // Set to NULL because arr hasn't been allocated yet

    printf("Building the initial food web...\n");
    
    // If quietMode is false, the program asks the user to input ogranisms names
    if (!quietMode) printf("Enter the name for an organism in the web (or enter DONE): ");
    char tempName[20] = "";
    scanf("%s",tempName); 
    if (!quietMode) printf("\n");

    // Loops until user inputs "Done"
    while (strcmp(tempName,"DONE") != 0) {
        // addOrgtoWb grows the pWeb arr to add new organsim
        addOrgToWeb(&pWeb,&numOrgs,tempName);
        // If debugMode is True, it will print current state of the food web
        if (debugMode) {
            printf("DEBUG MODE - added an organism:\n");
            printWeb(pWeb,numOrgs);
            printf("\n");
        }
        if (!quietMode) printf("Enter the name for an organism in the web (or enter DONE): ");
        scanf("%s",tempName); 
        if (!quietMode) printf("\n");
    }
    if (!quietMode) printf("\n");

    // If quietMode is False, prints these
    if (!quietMode) printf("Enter the pair of indices for a predator/prey relation.\n");
    if (!quietMode) printf("Enter any invalid index when done (-1 2, 0 -9, 3 3, etc.).\n");
    if (!quietMode) printf("The format is <predator index> <prey index>: ");
        
    int predInd, preyInd;
    // Reads two ints, represneting predator and prey indices
    scanf("%d %d",&predInd, &preyInd);
    if (!quietMode) printf("\n");

    // Loops until valid predator-prey indices are enetered 
    // (Indices are in bounds and predator not preying on itself)
    while (predInd >= 0 && preyInd >= 0 && predInd < numOrgs &&  preyInd < numOrgs && predInd != preyInd) {
        addRelationToWeb(pWeb,numOrgs,predInd,preyInd); // Adds this relationship to the correct organism's prey array
        // If debugMode is True, it will print current state of the food web
        if (debugMode) {
            printf("DEBUG MODE - added a relation:\n");
            printWeb(pWeb,numOrgs);
            printf("\n");
        }
        if (!quietMode) printf("Enter the pair of indices for a predator/prey relation.\n");
        if (!quietMode) printf("Enter any invalid index when done (-1 2, 0 -9, 3 3, etc.).\n");
        if (!quietMode) printf("The format is <predator index> <prey index>: ");
        
        scanf("%d %d",&predInd, &preyInd);  
        if (!quietMode) printf("\n");
    }
    printf("\n");

    printf("--------------------------------\n\n");
    printf("Initial food web complete.\n");
    printf("Displaying characteristics for the initial food web...\n");
    
    displayAll(pWeb,numOrgs,false); // Prints the characteristics of the web (apex predator, producers and etc.)

    // If basicMode is false, user gets more options
    if (!basicMode) {
        printf("--------------------------------\n\n");
        printf("Modifying the food web...");
        char opt = '?';

        while (opt != 'q') {
            if (!quietMode) {
                printf("Web modification options:\n");
                printf("   o = add a new organism (expansion)\n");
                printf("   r = add a new predator/prey relation (supplementation)\n");
                printf("   x = remove an organism (extinction)\n");
                printf("   p = print the updated food web\n");
                printf("   d = display ALL characteristics for the updated food web\n");
                printf("   q = quit\n");
                printf("Enter a character (o, r, x, p, d, or q): ");
            }
            scanf(" %c", &opt);
            printf("\n\n");

            // If user inputs 'o', they get to add a new organism
            if (opt == 'o') {
                char newName[20];
                if (!quietMode) printf("EXPANSION - enter the name for the new organism: ");
                scanf("%s",newName);
                if (!quietMode) printf("\n");
                printf("Species Expansion: %s\n", newName);
                addOrgToWeb(&pWeb,&numOrgs,newName);
                printf("\n");

                if (debugMode) {
                    printf("DEBUG MODE - added an organism:\n");
                    printWeb(pWeb,numOrgs);
                    printf("\n");
                }

            } 
            // If the user input 'x', they get to remove an organims from the web
            else if (opt == 'x') {
                int extInd;
                if (!quietMode) printf("EXTINCTION - enter the index for the extinct organism: ");
                scanf("%d",&extInd);
                if (!quietMode) printf("\n");
                if (extInd >= 0 && extInd < numOrgs) {
                    printf("Species Extinction: %s\n", pWeb[extInd].name);
                    removeOrgFromWeb(&pWeb,&numOrgs,extInd);
                } else {
                    printf("Invalid index for species extinction\n");
                }
                printf("\n");
                
                if (debugMode) {
                    printf("DEBUG MODE - removed an organism:\n");
                    printWeb(pWeb,numOrgs);
                    printf("\n");
                }

            } 
            // If the user input 'r', they get to add a new predator-prey relationship
            else if (opt == 'r') {
                int extInd;
                if (!quietMode) printf("SUPPLEMENTATION - enter the pair of indices for the new predator/prey relation.\n");
                if (!quietMode) printf("The format is <predator index> <prey index>: ");
                scanf("%d %d",&predInd, &preyInd);
                if (!quietMode) printf("\n");

                if (addRelationToWeb(pWeb,numOrgs,predInd,preyInd)) {
                    printf("New Food Source: %s eats %s\n", pWeb[predInd].name, pWeb[preyInd].name);
                };
                printf("\n");
                if (debugMode) {
                    printf("DEBUG MODE - added a relation:\n");
                    printWeb(pWeb,numOrgs);
                    printf("\n");
                }

            } // If the user input 'p', it prints the new updates web
            else if (opt == 'p') {
                printf("UPDATED Food Web Predators & Prey:\n");
                printWeb(pWeb,numOrgs);
                printf("\n");
                
            } // If the user input 'd', it displays the characteristics for the UPDATED food web
            else if (opt == 'd') {
                printf("Displaying characteristics for the UPDATED food web...\n\n");
                displayAll(pWeb,numOrgs,true);

            }
            printf("--------------------------------");
        
        }
        
    }

    freeWeb(pWeb,numOrgs); // Called to free all allocated memory associated with the web to prevent memory leaks

    return 0;
}

