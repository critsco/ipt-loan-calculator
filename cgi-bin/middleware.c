// loan_calculator.c
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

double calculate_monthly_payment(double principal, double rate, int months) {
    double monthly_rate = rate / 1200;
    return principal * monthly_rate * pow(1 + monthly_rate, months) / (pow(1 + monthly_rate, months) - 1);
}

int main() {
    char *input = NULL;
    size_t input_size = 0;
    ssize_t input_length = getline(&input, &input_size, stdin);

    double principal, rate;
    int years;
    sscanf(input, "%lf %lf %d", &principal, &rate, &years);

    double monthly_payment = calculate_monthly_payment(principal, rate, years);

    json_t *result = json_pack("{s:f}", "monthly_payment", monthly_payment);
    char *result_str = json_dumps(result, JSON_INDENT(2));
    printf("Content-Type: application/json\n\n%s\n", result_str);

    free(result_str);
    json_decref(result);

    free(input);

    return 0;
}
