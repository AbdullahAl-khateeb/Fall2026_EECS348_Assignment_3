/*
 * ============================================================================
 * Program Name: EECS 348 Assignment 3
 *
 * Description:
 * This C++ program prioritizes emails for a company CEO using an
 * object-oriented, list-based MaxHeap priority queue. Emails are prioritized
 * first by sender category and then by date, with newer emails from the same
 * sender category receiving higher priority.
 *
 * The program processes EMAIL, NEXT, READ, and COUNT commands from standard
 * input or from an optional input file.
 *
 * Inputs:
 * EMAIL <sender category>,<subject line>,<date>
 * NEXT
 * READ
 * COUNT
 *
 * Outputs:
 * NEXT displays the highest-priority unread email without removing it.
 * READ removes the highest-priority unread email without displaying it.
 * COUNT displays the current number of unread emails.
 *
 * Collaborators:
 * None.
 *
 * Other Sources:
 * ChatGPT - generated the baseline program and assisted with revisions,
 * testing strategy, comments, and maintainability improvements.
 * Google Gemini - generated the second GenAI program used for comparison.
 *
 * Author:
 * Abdullah Hamad Al-khateeb
 *
 * Creation Date:
 * October 1, 2026
 *
 * Revision Date:
 * October 1, 2026
 *
 * Revisions:
 * Improved comments and source attribution.
 * Preserved object-oriented design using Email, MaxHeap, and
 * EmailPriorityProgram classes.
 * Kept a vector-based list implementation for the custom MaxHeap.
 * Preserved safe handling of empty NEXT and READ commands.
 * ============================================================================
 */

#include <iostream> // Provides standard input and output streams.
#include <fstream>  // Provides file input streams.
#include <sstream>  // Provides string stream processing.
#include <string>   // Provides the std::string class.
#include <vector>   // Provides the list-based storage used by the MaxHeap.

/*
 * ============================================================================
 * Class: Email
 * Source: ChatGPT baseline, revised with ChatGPT assistance.
 *
 * Stores the information for one email and contains the behavior needed to
 * determine its priority relative to another Email object.
 * ============================================================================
 */
class Email {
private:
    std::string sender;  // Stores the sender category.
    std::string subject; // Stores the subject line.
    std::string date;    // Stores the original MM-DD-YYYY date.

    int categoryPriority; // Stores numeric sender priority.
    int dateValue;        // Stores date as YYYYMMDD for easy comparison.
    long arrivalId;       // Stores insertion order for deterministic ties.

    /*
     * Converts a sender category into a numeric priority.
     * Higher numbers represent higher-priority categories.
     */
    int determineCategoryPriority(const std::string& category) const {

        // Boss emails have the highest priority.
        if (category == "Boss") {
            return 5;
        }

        // Subordinate emails have the second-highest priority.
        if (category == "Subordinate") {
            return 4;
        }

        // Peer emails have the third-highest priority.
        if (category == "Peer") {
            return 3;
        }

        // ImportantPerson emails have the fourth-highest priority.
        if (category == "ImportantPerson") {
            return 2;
        }

        // OtherPerson emails have the lowest required priority.
        if (category == "OtherPerson") {
            return 1;
        }

        // Returns zero if an unexpected category is encountered.
        return 0;
    }

    /*
     * Converts MM-DD-YYYY into YYYYMMDD so normal integer comparison
     * correctly determines which date is newer.
     */
    int convertDate(const std::string& dateString) const {

        int month = 0; // Stores the parsed month.
        int day = 0;   // Stores the parsed day.
        int year = 0;  // Stores the parsed year.

        char dash1; // Stores the first '-' separator.
        char dash2; // Stores the second '-' separator.

        // Creates a stream from the date string.
        std::stringstream stream(dateString);

        // Extracts the month, day, and year.
        stream >> month >> dash1 >> day >> dash2 >> year;

        // Converts the date into sortable YYYYMMDD form.
        return (year * 10000) + (month * 100) + day;
    }

public:
    /*
     * Constructor
     * Initializes one Email object from the input fields.
     */
    Email(const std::string& senderValue,
          const std::string& subjectValue,
          const std::string& dateValueString,
          long arrivalValue)
        : sender(senderValue),
          subject(subjectValue),
          date(dateValueString),
          categoryPriority(determineCategoryPriority(senderValue)),
          dateValue(convertDate(dateValueString)),
          arrivalId(arrivalValue) {
    }

    /*
     * Determines whether this Email has higher priority than another Email.
     */
    bool hasHigherPriorityThan(const Email& other) const {

        // Sender category is the primary priority.
        if (categoryPriority != other.categoryPriority) {
            return categoryPriority > other.categoryPriority;
        }

        // Newer date is the secondary priority.
        if (dateValue != other.dateValue) {
            return dateValue > other.dateValue;
        }

        // If category and date tie, the earlier arrival is kept first.
        return arrivalId < other.arrivalId;
    }

    // Returns the sender category.
    const std::string& getSender() const {
        return sender;
    }

    // Returns the subject line.
    const std::string& getSubject() const {
        return subject;
    }

    // Returns the original date string.
    const std::string& getDate() const {
        return date;
    }
};

/*
 * ============================================================================
 * Class: MaxHeap
 * Source: ChatGPT baseline, revised with ChatGPT assistance.
 *
 * Implements a custom list-based binary MaxHeap using std::vector only as
 * storage. No pre-existing heap module or heap algorithm is used.
 * ============================================================================
 */
class MaxHeap {
private:
    std::vector<Email> heap; // Stores the Email objects in heap order.

    /*
     * Swaps two Email objects in the underlying list.
     */
    void swapEmails(int first, int second) {

        // Temporarily stores the first email.
        Email temp = heap[first];

        // Copies the second email into the first position.
        heap[first] = heap[second];

        // Copies the saved email into the second position.
        heap[second] = temp;
    }

    /*
     * Restores the MaxHeap property after an insertion.
     */
    void heapifyUp(int index) {

        // Continues until the item reaches the root or no swap is needed.
        while (index > 0) {

            // Calculates the parent index.
            int parent = (index - 1) / 2;

            // Stops when the child does not outrank its parent.
            if (!heap[index].hasHigherPriorityThan(heap[parent])) {
                break;
            }

            // Moves the higher-priority child upward.
            swapEmails(index, parent);

            // Continues checking from the parent's old position.
            index = parent;
        }
    }

    /*
     * Restores the MaxHeap property after removing the root.
     */
    void heapifyDown(int index) {

        // Continues until the current item is correctly positioned.
        while (true) {

            // Calculates the left-child index.
            int left = (2 * index) + 1;

            // Calculates the right-child index.
            int right = (2 * index) + 2;

            // Assumes the current node has the highest priority.
            int largest = index;

            // Checks whether the left child exists and has higher priority.
            if (left < static_cast<int>(heap.size()) &&
                heap[left].hasHigherPriorityThan(heap[largest])) {

                // Records the left child as the highest-priority position.
                largest = left;
            }

            // Checks whether the right child exists and has higher priority.
            if (right < static_cast<int>(heap.size()) &&
                heap[right].hasHigherPriorityThan(heap[largest])) {

                // Records the right child as the highest-priority position.
                largest = right;
            }

            // Stops when the current item already has highest priority.
            if (largest == index) {
                break;
            }

            // Swaps the current item with its higher-priority child.
            swapEmails(index, largest);

            // Continues from the child's previous position.
            index = largest;
        }
    }

public:
    /*
     * Inserts one Email into the MaxHeap.
     */
    void insert(const Email& email) {

        // Adds the email to the end of the list.
        heap.push_back(email);

        // Restores the MaxHeap property.
        heapifyUp(static_cast<int>(heap.size()) - 1);
    }

    /*
     * Returns the highest-priority email without removing it.
     */
    const Email* peek() const {

        // Returns null when the inbox is empty.
        if (heap.empty()) {
            return nullptr;
        }

        // The highest-priority email is always at index zero.
        return &heap[0];
    }

    /*
     * Removes the highest-priority email.
     */
    void removeMax() {

        // Safely does nothing when there are no emails.
        if (heap.empty()) {
            return;
        }

        // If only one email exists, remove it and finish.
        if (heap.size() == 1) {
            heap.pop_back();
            return;
        }

        // Moves the final email into the root position.
        heap[0] = heap.back();

        // Removes the duplicate final element.
        heap.pop_back();

        // Restores the MaxHeap property.
        heapifyDown(0);
    }

    /*
     * Returns the number of unread emails.
     */
    int count() const {

        // Converts vector size to int for output.
        return static_cast<int>(heap.size());
    }
};

/*
 * ============================================================================
 * Class: EmailPriorityProgram
 * Source: ChatGPT baseline, revised with ChatGPT assistance.
 *
 * Controls command parsing and uses the MaxHeap object to manage the inbox.
 * ============================================================================
 */
class EmailPriorityProgram {
private:
    MaxHeap inbox;      // Stores the CEO's unread emails.
    long arrivalCounter; // Gives each inserted email a unique arrival order.

    /*
     * Parses the sender, subject, and date from an EMAIL command.
     */
    bool parseEmail(const std::string& line,
                    std::string& sender,
                    std::string& subject,
                    std::string& date) const {

        // Removes the initial "EMAIL " portion of the command.
        std::string payload = line.substr(6);

        // Finds the comma after the sender category.
        std::size_t firstComma = payload.find(',');

        // Stops if the first required delimiter is missing.
        if (firstComma == std::string::npos) {
            return false;
        }

        // Finds the comma after the subject.
        std::size_t secondComma = payload.find(',', firstComma + 1);

        // Stops if the second required delimiter is missing.
        if (secondComma == std::string::npos) {
            return false;
        }

        // Extracts the sender category.
        sender = payload.substr(0, firstComma);

        // Extracts the subject line.
        subject = payload.substr(firstComma + 1,
                                 secondComma - firstComma - 1);

        // Extracts the date.
        date = payload.substr(secondComma + 1);

        // Reports successful parsing.
        return true;
    }

    /*
     * Processes one EMAIL command.
     */
    void processEmailCommand(const std::string& line) {

        std::string sender;  // Stores the parsed sender category.
        std::string subject; // Stores the parsed subject.
        std::string date;    // Stores the parsed date.

        // Inserts the email only when all fields were parsed.
        if (parseEmail(line, sender, subject, date)) {

            // Creates an Email object from the parsed fields.
            Email email(sender, subject, date, arrivalCounter);

            // Advances the arrival counter for the next email.
            arrivalCounter++;

            // Inserts the new object into the MaxHeap.
            inbox.insert(email);
        }
    }

    /*
     * Processes NEXT without removing the email.
     */
    void processNextCommand() const {

        // Gets the current highest-priority email.
        const Email* nextEmail = inbox.peek();

        // Prints nothing when the inbox is empty.
        if (nextEmail == nullptr) {
            return;
        }

        // Prints the required NEXT heading.
        std::cout << "Next email:" << std::endl;

        // Prints the sender category.
        std::cout << "Sender: "
                  << nextEmail->getSender()
                  << std::endl;

        // Prints the subject.
        std::cout << "Subject: "
                  << nextEmail->getSubject()
                  << std::endl;

        // Prints the date.
        std::cout << "Date: "
                  << nextEmail->getDate()
                  << std::endl;
    }

    /*
     * Processes READ by removing the current highest-priority email.
     */
    void processReadCommand() {

        // removeMax() safely does nothing when the inbox is empty.
        inbox.removeMax();
    }

    /*
     * Processes COUNT.
     */
    void processCountCommand() const {

        // Prints the exact required unread-email count format.
        std::cout << "There are "
                  << inbox.count()
                  << " emails to read."
                  << std::endl;
    }

public:
    /*
     * Constructor
     * Starts the email arrival counter at zero.
     */
    EmailPriorityProgram()
        : arrivalCounter(0) {
    }

    /*
     * Reads and processes every command from the selected input stream.
     */
    void run(std::istream& input) {

        std::string line; // Stores one command at a time.

        // Continues until there are no more command lines.
        while (std::getline(input, line)) {

            // Removes a carriage return when reading Windows-style files.
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            // Processes an EMAIL command.
            if (line.rfind("EMAIL ", 0) == 0) {
                processEmailCommand(line);

            // Processes a NEXT command.
            } else if (line == "NEXT") {
                processNextCommand();

            // Processes a READ command.
            } else if (line == "READ") {
                processReadCommand();

            // Processes a COUNT command.
            } else if (line == "COUNT") {
                processCountCommand();
            }
        }
    }
};

/*
 * ============================================================================
 * Function: main
 * Source: ChatGPT baseline, revised with ChatGPT assistance.
 *
 * Creates the main program object and selects either a test file or standard
 * input as the command source.
 * ============================================================================
 */
int main(int argc, char* argv[]) {

    // Creates the object that controls the complete application.
    EmailPriorityProgram program;

    // Checks whether a test filename was supplied.
    if (argc > 1) {

        // Opens the supplied input file.
        std::ifstream inputFile(argv[1]);

        // Stops when the input file could not be opened.
        if (!inputFile.is_open()) {
            return 1;
        }

        // Processes commands from the file.
        program.run(inputFile);

    } else {

        // Processes commands from standard input when no file was supplied.
        program.run(std::cin);
    }

    // Ends the program successfully.
    return 0;
}
