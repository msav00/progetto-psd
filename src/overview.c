#include "overview.h"

#include <stdlib.h>

Overview *newOverview() {
    return calloc(1, sizeof(Overview));
}

void deleteOverview(Overview *overview) {
    free(overview);
}
