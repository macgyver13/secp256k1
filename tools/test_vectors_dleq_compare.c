/* Temporary migration check for the DLEQ test vector format.
 *
 * Generate the old header and run this comparator from the repository root:
 *
 *   cc -std=c89 -Wall -Wextra -Werror \
 *     tools/test_vectors_dleq_compare.c -o /tmp/dleq_vectors_compare
 *   /tmp/dleq_vectors_compare
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#include "../src/modules/dleq/vectors.h"
#include "../src/modules/dleq/dleq_vectors.h"

static int is_zero(const unsigned char *data, size_t len) {
    static const unsigned char zero[64] = { 0 };
    assert(len <= sizeof(zero));
    return memcmp(data, zero, len) == 0;
}

int main(void) {
    size_t i;
    size_t j = 0;

    assert(ARRAY_SIZE(a_bytes) == ARRAY_SIZE(dleq_generate_vectors));
    for (i = 0; i < ARRAY_SIZE(a_bytes); ++i) {
        const struct dleq_generate_vector *vector = &dleq_generate_vectors[i];

        assert(memcmp(a_bytes[i], vector->scalar_a, sizeof(vector->scalar_a)) == 0);
        assert(memcmp(B_bytes[i], vector->point_B, sizeof(vector->point_B)) == 0);
        assert(memcmp(auxrand_bytes[i], vector->auxrand, sizeof(vector->auxrand)) == 0);
        assert(memcmp(msg_bytes[i], vector->message, sizeof(vector->message)) == 0);
        assert(memcmp(proof_bytes[i], vector->expected_proof, sizeof(vector->expected_proof)) == 0);
        assert(is_zero(B_bytes[i], sizeof(B_bytes[i])) == vector->point_B_is_infinity);
        assert((!is_zero(msg_bytes[i], sizeof(msg_bytes[i]))) == vector->has_msg);
        assert(success[i] == vector->expected_success);
    }

    for (i = 0; i < ARRAY_SIZE(A_bytes); ++i) {
        const struct dleq_verify_vector *vector;

        if (is_zero(A_bytes[i], sizeof(A_bytes[i])) || is_zero(C_bytes[i], sizeof(C_bytes[i]))) {
            continue;
        }
        assert(j < ARRAY_SIZE(dleq_verify_vectors));
        vector = &dleq_verify_vectors[j++];
        assert(memcmp(A_bytes[i], vector->point_A, sizeof(vector->point_A)) == 0);
        assert(memcmp(B_bytes[i], vector->point_B, sizeof(vector->point_B)) == 0);
        assert(memcmp(C_bytes[i], vector->point_C, sizeof(vector->point_C)) == 0);
        assert(memcmp(msg_bytes[i], vector->message, sizeof(vector->message)) == 0);
        assert(memcmp(proof_bytes[i], vector->proof, sizeof(vector->proof)) == 0);
        assert((!is_zero(msg_bytes[i], sizeof(msg_bytes[i]))) == vector->has_msg);
        assert(success[i] == vector->expected_success);
    }
    assert(j == ARRAY_SIZE(dleq_verify_vectors));
    return 0;
}
