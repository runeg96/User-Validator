#include "app/app_summary.hpp"

#include <iostream>

std::string buildSummaryLogMessage(const AppState& i_state)
{
    return "Run summary: parsed=" + std::to_string(i_state.users.size()) + ", skipped=" + std::to_string(i_state.rejectedRecords.size()) + ", valid=" + std::to_string(i_state.validUsers.size()) + ", invalid=" + std::to_string(i_state.invalidUsers.size());
}

void printSummary(const AppState& i_state)
{
    std::cout << "Parsed " << i_state.users.size() << " users\n";
    std::cout << "Skipped during parser/schema: " << i_state.rejectedRecords.size() << "\n";
    std::cout << "Valid users: " << i_state.validUsers.size() << "\n";
    std::cout << "Invalid users: " << i_state.invalidUsers.size() << "\n";
    std::cout << "Parsing took " << i_state.parseDurationUs << " us\n";
    std::cout << "Validation took " << i_state.validationDurationUs << " us\n";
}