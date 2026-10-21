#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 256

void printTable(int **dp, int n, int W, int currentRow) {
    printf("\n       DP Table (after item %d)\n", currentRow);
    printf("Items  ");

    for (int w = 0; w <= W; w++)
        printf("%4d", w);

    printf("\n");

    for (int i = 0; i <= currentRow; i++) {
        if (i == 0)
            printf("   0   ");
        else
            printf("Item %-2d", i);

        for (int w = 0; w <= W; w++)
            printf("%4d", dp[i][w]);

        printf("\n");
    }
}

int main(void) {
    FILE *input = fopen("knapsack_test.txt", "r");
    FILE *output = fopen("result.txt", "w");

    if (input == NULL || output == NULL) {
        printf("Error opening input.txt or result.txt.\n");

        if (input != NULL) fclose(input);
        if (output != NULL) fclose(output);

        return 1;
    }

    char line[LINE_SIZE];
    int testCase = 0;

    while (fgets(line, sizeof(line), input) != NULL) {
        // Find the start of a test case.
        if (strncmp(line, "#Test", 5) != 0)
            continue;

        int n, W;

        // Read the next non-empty line containing n and W.
        int found = 0;

        while (fgets(line, sizeof(line), input) != NULL) {
            if (sscanf(line, "%d %d", &n, &W) == 2) {
                found = 1;
                break;
            }

            if (line[0] == '#')
                break;
        }

        if (!found || n < 0 || W < 0) {
            fprintf(stderr, "Invalid test case input.\n");
            break;
        }

        int *weights = malloc((size_t)n * sizeof(int));
        int *values = malloc((size_t)n * sizeof(int));
        int **dp = malloc((size_t)(n + 1) * sizeof(int *));

        if ((n > 0 && (weights == NULL || values == NULL))
            || dp == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            free(weights);
            free(values);
            free(dp);
            break;
        }

        int valid = 1;

        for (int i = 0; i < n; i++) {
            if (fgets(line, sizeof(line), input) == NULL ||
                sscanf(line, "%d %d", &weights[i], &values[i]) != 2 ||
                weights[i] < 0) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            fprintf(stderr, "Invalid item data.\n");
            free(weights);
            free(values);
            free(dp);
            break;
        }

        for (int i = 0; i <= n; i++) {
            dp[i] = calloc((size_t)W + 1, sizeof(int));

            if (dp[i] == NULL) {
                valid = 0;
                for (int j = 0; j < i; j++)
                    free(dp[j]);
                break;
            }
        }

        if (!valid) {
            fprintf(stderr, "Memory allocation failed.\n");
            free(weights);
            free(values);
            free(dp);
            break;
        }

        testCase++;
        printf("\n========== Test Case %d ==========\n", testCase);
        printf("Items = %d, Capacity = %d\n", n, W);

        printTable(dp, n, W, 0);

        for (int i = 1; i <= n; i++) {
            for (int w = 0; w <= W; w++) {
                if (weights[i - 1] > w) {
                    dp[i][w] = dp[i - 1][w];
                } else {
                    int exclude = dp[i - 1][w];
                    int include = values[i - 1]
                                + dp[i - 1][w - weights[i - 1]];

                    dp[i][w] = (include > exclude)
                             ? include : exclude;
                }
            }

            printf("\nProcessing item %d (weight=%d, value=%d)\n",
                   i, weights[i - 1], values[i - 1]);

            printTable(dp, n, W, i);
        }

        int answer = dp[n][W];

        printf("\nMaximum value = %d\n", answer);

        fprintf(output, "#Test case input (%d %d) %d\n",
                n, W, answer);

        for (int i = 0; i <= n; i++)
            free(dp[i]);

        free(dp);
        free(weights);
        free(values);
    }

    fclose(input);
    fclose(output);

    printf("\nAll results saved in result.txt\n");

    return 0;
}
