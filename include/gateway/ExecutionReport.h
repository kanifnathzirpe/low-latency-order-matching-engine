#ifndef EXECUTIONREPORT_H
#define EXECUTIONREPORT_H

#include <string>

enum class ExecutionStatus
{
    Accepted,
    Rejected,
    PartiallyFilled,
    Filled
};

class ExecutionReport
{
private:
    ExecutionStatus status;
    std::string message;

public:
    ExecutionReport(
        ExecutionStatus status,
        const std::string& message
    );

    ExecutionStatus getStatus() const;

    std::string getMessage() const;

    std::string toString() const;
};

#endif