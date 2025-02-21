#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void generateRandomString(char *str, int length) {
    for (int i = 0; i < length; i++) {
        str[i] = 'A' + rand() % 26; // Random letter from A-Z
    }
    str[length] = '\0'; // Null-terminate the string
}

// Function to implement the Naive String Matching Algorithm
void naiveStringMatch(char *text, char *pattern) {
    int n = strlen(text);    // Length of the text
    int m = strlen(pattern); // Length of the pattern

    // Loop through every possible position in the text where the pattern might match
    for (int i = 0; i <= n - m; i++) {
        int j = 0;

        // Compare the pattern with the substring starting at position i
        while (j < m && text[i + j] == pattern[j]) {
            j++;
        }

        // If we have checked all characters of the pattern, we found a match
        if (j == m) {
            printf("Pattern found at index %d\n", i);
        }
    }
}

#define NO_OF_CHARS 256 // Number of characters in the ASCII character set

// Function to preprocess the pattern and create the "bad character" shift table
void badCharacterHeuristic(char *pattern, int m, int badChar[]) {
    // Initialize all occurrences as -1
    for (int i = 0; i < NO_OF_CHARS; i++) {
        badChar[i] = -1;
    }

    // Fill the actual value of last occurrence of each character in the pattern
    for (int i = 0; i < m; i++) {
        badChar[(int)pattern[i]] = i;
    }
}

// Function to perform the Boyer-Moore search
void boyerMooreSearch(char *text, char *pattern) {
    int n = strlen(text);
    int m = strlen(pattern);

    // Create the "bad character" heuristic table
    int badChar[NO_OF_CHARS];
    badCharacterHeuristic(pattern, m, badChar);

    int s = 0; // Shift of the pattern with respect to text

    // Loop to slide the pattern over the text one by one
    while (s <= (n - m)) {
        int j = m - 1;

        // Keep reducing j while the characters of the pattern match the text
        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }

        // If the pattern is found (j < 0), print the index of match
        if (j < 0) {
            printf("Pattern found at index %d\n", s);

            // Shift the pattern to align with the next possible match
            s += (s + m < n) ? m - badChar[text[s + m]] : 1;
        } else {
            // Shift the pattern to the right based on the bad character heuristic
            s += (j - badChar[text[s + j]] > 1) ? j - badChar[text[s + j]] : 1;
        }
    }
}

// Function to compute the Longest Prefix Suffix (LPS) array
void computeLPSArray(char *pattern, int m, int *lps) {
    int length = 0; // Length of the previous longest prefix suffix
    lps[0] = 0;     // LPS[0] is always 0

    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            // Mismatch after length matches
            if (length != 0) {
                length = lps[length - 1]; // Use the previous longest prefix suffix
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

// Function to perform the Knuth-Morris-Pratt (KMP) search algorithm
void KMPSearch(char *text, char *pattern) {
    int n = strlen(text);
    int m = strlen(pattern);

    // Compute the LPS array for the pattern
    int lps[m];
    computeLPSArray(pattern, m, lps);

    int i = 0; // index for text
    int j = 0; // index for pattern

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            printf("Pattern found at index %d\n", i - j);
            j = lps[j - 1]; // Use LPS to continue searching
        }
        // Mismatch after j matches
        else if (i < n && pattern[j] != text[i]) {
            // Don't need to compare pattern[0..lps[j-1]] again
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
}

#define d 256 // Number of characters in the input alphabet (ASCII)
#define q 101 // A prime number for modulus to reduce hash value

// Function to perform Rabin-Karp search
void rabinKarpSearch(char *text, char *pattern) {
    int n = strlen(text);
    int m = strlen(pattern);

    int i, j;
    int patternHash = 0; // Hash value for the pattern
    int textHash = 0;    // Hash value for the text
    int h = 1;           // d^(m-1) % q (The value of d^m % q)

    // Compute the value of h = d^(m-1) % q
    for (i = 0; i < m - 1; i++) {
        h = (h * d) % q;
    }

    // Compute the hash value for the pattern and first window of text
    for (i = 0; i < m; i++) {
        patternHash = (d * patternHash + pattern[i]) % q;
        textHash = (d * textHash + text[i]) % q;
    }

    // Slide the pattern over the text one by one
    for (i = 0; i <= n - m; i++) {
        // If the hash values match, check for actual match character by character
        if (patternHash == textHash) {
            // Check characters one by one
            for (j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    break;
                }
            }

            // If pattern is found, print the index
            if (j == m) {
                printf("Pattern found at index %d\n", i);
            }
        }

        // Calculate hash value for the next window of text
        if (i < n - m) {
            textHash = (d * (textHash - text[i] * h) + text[i + m]) % q;

            // If the hash value becomes negative, convert it to positive
            if (textHash < 0) {
                textHash = textHash + q;
            }
        }
    }
}

// Performance Testing Function
void test_perf(int text_length, int pattern_length) {
    // Generate random text and pattern
    char *text = (char *)malloc(text_length + 1);
    char *pattern = (char *)malloc(pattern_length + 1);

    generateRandomString(text, text_length);
    generateRandomString(pattern, pattern_length);

    // Measure the time taken by each algorithm
    clock_t start, end;

    printf("Testing with text length %d and pattern length %d...\n", text_length, pattern_length);

    // Naive String Matching
    start = clock();
    naiveStringMatch(text, pattern);
    end = clock();
    printf("Naive String Matching took %f seconds.\n", (double)(end - start) / CLOCKS_PER_SEC);

    // Boyer-Moore
    start = clock();
    boyerMooreSearch(text, pattern);
    end = clock();
    printf("Boyer-Moore String Matching took %f seconds.\n", (double)(end - start) / CLOCKS_PER_SEC);

    // Knuth-Morris-Pratt (KMP)
    start = clock();
    KMPSearch(text, pattern);
    end = clock();
    printf("KMP String Matching took %f seconds.\n", (double)(end - start) / CLOCKS_PER_SEC);

    // Rabin-Karp
    start = clock();
    rabinKarpSearch(text, pattern);
    end = clock();
    printf("Rabin-Karp String Matching took %f seconds.\n", (double)(end - start) / CLOCKS_PER_SEC);

    // Free allocated memory
    free(text);
    free(pattern);
}

int main() {
    // Test Case 1: Multiple Matches in the Text
    char text[] = "ABABABCA";
    char pattern[] = "ABAB";

    printf("Test Case 1: Multiple Matches in the Text\n");
    printf("Text: %s\n", text);
    printf("Pattern: %s\n\n", pattern);

    printf("Naive String Matching:\n");
    naiveStringMatch(text, pattern);

    printf("\nBoyer-Moore String Matching:\n");
    boyerMooreSearch(text, pattern);

    printf("\nKnuth-Morris-Pratt (KMP) String Matching:\n");
    KMPSearch(text, pattern);

    printf("\nRabin-Karp String Matching:\n");
    rabinKarpSearch(text, pattern);

    printf("\n========================\n\n");

    // Test Case 2: No Matches in the Text
    char text2[] = "HELLO_WORLD";
    char pattern2[] = "WORLDWIDE";

    printf("Test Case 2: No Matches in the Text\n");
    printf("Text: %s\n", text2);
    printf("Pattern: %s\n\n", pattern2);

    printf("Naive String Matching:\n");
    naiveStringMatch(text2, pattern2);

    printf("\nBoyer-Moore String Matching:\n");
    boyerMooreSearch(text2, pattern2);

    printf("\nKnuth-Morris-Pratt (KMP) String Matching:\n");
    KMPSearch(text2, pattern2);

    printf("\nRabin-Karp String Matching:\n");
    rabinKarpSearch(text2, pattern2);

    printf("\n========================\n\n");

    // Test performance with large strings
    int text_length = 1000000; // 1 million characters
    int pattern_length = 10;   // A short pattern

    test_perf(text_length, pattern_length);

    return 0;
}
