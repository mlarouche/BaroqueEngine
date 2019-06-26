#pragma once

#define BAROQUE_CONCAT_IMPL(x, y) x##y
#define BAROQUE_CONCAT(x, y) BAROQUE_CONCAT_IMPL(x, y)

#define BAROQUE_UNUSED(variable) (void)(variable)

#define BAROQUE_UNIQUE_VARIABLE(variable) BAROQUE_CONCAT(variable, BAROQUE_UNIQUE_COUNTER)