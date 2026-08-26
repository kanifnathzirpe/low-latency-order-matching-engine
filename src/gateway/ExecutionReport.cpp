#include "gateway/ExecutionReport.h"

ExecutionReport::ExecutionReport(
    ExecutionStatus status,
    const std::string& message)
    : status(status),
      message(message)
{
}

ExecutionStatus ExecutionReport::getStatus() const
{
    return status;
}

std::string ExecutionReport::getMessage() const
{
    return message;
}

std::string ExecutionReport::toString() const
{
    std::string statusString;

    switch (status)
    {
        case ExecutionStatus::Accepted:
            statusString = "ACCEPTED";
            break;

        case ExecutionStatus::Rejected:
            statusString = "REJECTED";
            break;

        case ExecutionStatus::PartiallyFilled:
            statusString = "PARTIALLY_FILLED";
            break;

        case ExecutionStatus::Filled:
            statusString = "FILLED";
            break;
    }

    if (message.empty())
    {
        return statusString;
    }

    return statusString + " " + message;
}