#pragma once

#include <string>

#include "app/app_state.hpp"

std::string buildSummaryLogMessage(const AppState& i_state);
void printSummary(const AppState& i_state);