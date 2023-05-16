#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <math.h>

typedef struct {
    int paymentNo;
    double principlePayable;
    double interestPayable;
    double totalAmortization;
    double interestRate;
} Payment;

Payment *calculate_amortization(double loanAmount, int term, double interestRate) {
    double monthlyRate = interestRate / 1200;
    double monthlyPayment = loanAmount * monthlyRate / (1 - pow(1 + monthlyRate, -term));
    Payment *payments = malloc(term * sizeof(Payment));
    double balance = loanAmount;
    for (int i = 0; i < term; i++) {
        payments[i].paymentNo = i + 1;
        payments[i].interestPayable = balance * monthlyRate;
        payments[i].principlePayable = monthlyPayment - payments[i].interestPayable;
        payments[i].totalAmortization = monthlyPayment;
        payments[i].interestRate = interestRate;
        balance -= payments[i].principlePayable;
    }
    return payments;
}

int main() {
    char *content_length_str = getenv("CONTENT_LENGTH");
    if (!content_length_str) {
        return 1;
    }

    int content_length = atoi(content_length_str);
    char *input = malloc(content_length + 1);
    fread(input, 1, content_length, stdin);
    input[content_length] = '\0';

    json_error_t error;
    json_t *root = json_loads(input, 0, &error);
    free(input);

    if (!root) {
        return 1;
    }

    double loanAmount = json_number_value(json_object_get(root, "loanAmount"));
    int term = json_integer_value(json_object_get(root, "term"));
    double interestRate = json_number_value(json_object_get(root, "interestRate"));
    json_decref(root);

    Payment *payments = calculate_amortization(loanAmount, term, interestRate);

    json_t *result = json_array();
    for (int i = 0; i < term; i++) {
        json_t *payment = json_object();
        json_object_set_new(payment, "paymentNo", json_integer(payments[i].paymentNo));
        json_object_set_new(payment, "principlePayable", json_real(payments[i].principlePayable));
        json_object_set_new(payment, "interestPayable", json_real(payments[i].interestPayable));
        json_object_set_new(payment, "totalAmortization", json_real(payments[i].totalAmortization));
        json_object_set_new(payment, "interestRate", json_real(payments[i].interestRate));
        json_array_append_new(result, payment);
    }
    free(payments);

    char *output = json_dumps(result, JSON_INDENT(2));
    json_decref(result);

    printf("Content-Type: application/json\n\n");
    printf("%s", output);
    free(output);

    return 0;
}
