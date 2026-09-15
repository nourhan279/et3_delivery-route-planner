#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

const double MAX_CAPACITY = 10.0;
const double EPSILON = 1e-9;

//one delivery request
struct Delivery {
    int id;
    string area;
    int priority;
    double weight;
};

//one delivery trip
struct Trip {
    int tripNumber;
    vector<Delivery> deliveries;
    double totalWeight;
};


//Read deliveries from CSV
vector<Delivery> readDeliveriesFromCSV(const string& filename) {
    vector<Delivery> deliveries;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open file: " << filename << endl;
        return deliveries;
    }
    string line;
    //Skip CSV header
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string idText;
        string area;
        string priorityText;
        string weightText;

        getline(ss, idText, ',');
        getline(ss, area, ',');
        getline(ss, priorityText, ',');
        getline(ss, weightText, ',');

        try {
            Delivery delivery;
            delivery.id = stoi(idText);
            delivery.area = area;
            delivery.priority = stoi(priorityText);
            delivery.weight = stod(weightText);
            deliveries.push_back(delivery);
        }
        catch (...) {
            cerr << "Warning: Invalid row skipped: " << line << endl; //for any row that have error
        }
    }
    file.close();
    return deliveries;
}

//Validate deliveries
vector<Delivery> validateDeliveries(
    const vector<Delivery>& deliveries,
    vector<Delivery>& rejectedDeliveries
) {
    vector<Delivery> validDeliveries;
    for (const Delivery& delivery : deliveries) {
        bool valid = true;

        //weight must be positive(>0)
        if (delivery.weight <= 0) {
            valid = false;
            cerr << "Rejected delivery " << delivery.id
                 << ": weight must be greater than 0." << endl;
        }
        //single package cannot be more than 10 kilos
        else if (delivery.weight > MAX_CAPACITY + EPSILON) {
            valid = false;
            cerr << "Rejected delivery " << delivery.id
                 << ": package exceeds 10 kg." << endl;
        }
        //area cannot be empty
        else if (delivery.area.empty()) {
            valid = false;
            cerr << "Rejected delivery " << delivery.id
                 << ": area is empty." << endl;
        }
        //priority must be positive(>0)
        else if (delivery.priority <= 0) {
            valid = false;
            cerr << "Rejected delivery " << delivery.id
                 << ": priority must be greater than 0." << endl;
        }
        if (valid) {
            validDeliveries.push_back(delivery);
        }
        else {
            rejectedDeliveries.push_back(delivery);
        }
    }
    return validDeliveries;
}

//Sort deliveries according to priority first then id
void sortDeliveries(vector<Delivery>& deliveries) {
    sort(
        deliveries.begin(),
        deliveries.end(),
        [](const Delivery& a, const Delivery& b) {
            //Lower priority number = more important
            if (a.priority != b.priority) {
                return a.priority < b.priority;
            }
            //If priorities are equal then use id
            return a.id < b.id;
        }
    );
}

//Check if a delivery fits in a trip (total w<=10)
bool canFit(const Trip& trip, const Delivery& delivery) {
    return trip.totalWeight + delivery.weight
           <= MAX_CAPACITY + EPSILON;
}

//Check whether trip contains an area the same as the delivery's area
bool hasSameArea(const Trip& trip, const string& area) {
    for (const Delivery& delivery : trip.deliveries) {
        if (delivery.area == area) {
            return true;
        }
    }
    return false;
}


//Add delivery to trip
void addDeliveryToTrip(Trip& trip, const Delivery& delivery) {
    trip.deliveries.push_back(delivery);
    trip.totalWeight += delivery.weight;
}


//Find best same-area trip if possible
int findBestSameAreaTrip(const vector<Trip>& trips,const Delivery& delivery) {
    int bestTripIndex = -1;
    double smallestRemainingCapacity = MAX_CAPACITY + 1;

    for (int i = 0; i < static_cast<int>(trips.size()); i++) {
        const Trip& trip = trips[i];
        if (hasSameArea(trip, delivery.area) && canFit(trip, delivery)) {
            double remainingCapacity =
                MAX_CAPACITY - (trip.totalWeight + delivery.weight);
            //Choose the trip with the least remaining space
            if (remainingCapacity < smallestRemainingCapacity) {
                smallestRemainingCapacity = remainingCapacity;
                bestTripIndex = i;
            }
        }
    }
    return bestTripIndex;
}


//Find best trip with available capacity
int findBestAvailableTrip(const vector<Trip>& trips,const Delivery& delivery) {
    int bestTripIndex = -1;
    double smallestRemainingCapacity = MAX_CAPACITY + 1;

    for (int i = 0; i < static_cast<int>(trips.size()); i++) {
        const Trip& trip = trips[i];
        if (canFit(trip, delivery)) {
            double remainingCapacity =
                MAX_CAPACITY - (trip.totalWeight + delivery.weight);
            if (remainingCapacity < smallestRemainingCapacity) {
                smallestRemainingCapacity = remainingCapacity;
                bestTripIndex = i;
            }
        }
    }
    return bestTripIndex;
}


//Create delivery trips
vector<Trip> createTrips(const vector<Delivery>& deliveries) {
    vector<Trip> trips;

    for (const Delivery& delivery : deliveries) {
        //First option:Put delivery with another delivery from the same area
        int tripIndex = findBestSameAreaTrip(trips, delivery);

        //Second option:If no same-area trip fits, use any trip with capacity
        if (tripIndex == -1) {
            tripIndex = findBestAvailableTrip(trips, delivery);
        }

        //last option:If no existing trip can fit the delivery then create a new trip.
        if (tripIndex == -1) {
            Trip newTrip;
            newTrip.tripNumber = static_cast<int>(trips.size()) + 1;
            newTrip.totalWeight = 0.0;
            addDeliveryToTrip(newTrip, delivery);
            trips.push_back(newTrip);
        }
        else {
            addDeliveryToTrip(trips[tripIndex], delivery);
        }
    }
    return trips;
}

//Print trip
void printTrip(const Trip& trip) {
    cout << "Trip " << trip.tripNumber << endl;
    cout << "  Deliveries:" << endl;
    for (const Delivery& delivery : trip.deliveries) {
        cout << "    ID: " << delivery.id
             << " | Area: " << delivery.area
             << " | Priority: " << delivery.priority
             << " | Weight: "
             << fixed << setprecision(1)
             << delivery.weight << " kg"
             << endl;
    }

    cout << "  Total weight: "
         << fixed << setprecision(1)
         << trip.totalWeight
         << " kg" << endl;

    cout << "  Remaining capacity: "
         << fixed << setprecision(1)
         << MAX_CAPACITY - trip.totalWeight
         << " kg" << endl;

    cout << endl;
}


//Print all trips
void printTrips(const vector<Trip>& trips) {
    cout << "\n-------------------- DELIVERY PLAN ------------------\n\n";
    if (trips.empty()) {
        cout << "No trips were created.\n";
        return;
    }
    for (const Trip& trip : trips) {
        printTrip(trip);
    }
}


//Print summary (additional feature)
void printSummary(const vector<Delivery>& validDeliveries,const vector<Delivery>& rejectedDeliveries,const vector<Trip>& trips) {
    double totalWeight = 0.0;
    for (const Delivery& delivery : validDeliveries) {
        totalWeight += delivery.weight;
    }
    cout << "--------------- SUMMARY -------------\n";
    cout << "Valid deliveries: "
         << validDeliveries.size()
         << endl;
    cout << "Rejected deliveries: "
         << rejectedDeliveries.size()
         << endl;
    cout << "Total weight: "
         << fixed << setprecision(1)
         << totalWeight
         << " kg"
         << endl;
    cout << "Number of trips: "
         << trips.size()
         << endl;

    if (!trips.empty()) {
        double averageWeight =totalWeight / trips.size();
        cout << "Average trip weight: "
             << fixed << setprecision(1)
             << averageWeight
             << " kg"
             << endl;
    }
}

//Print rejected deliveries
void printRejectedDeliveries(
    const vector<Delivery>& rejectedDeliveries
) {
    if (rejectedDeliveries.empty()) {
        return;
    }
    cout << "\n-------------- REJECTED DELIVERIES --------------\n";
    for (const Delivery& delivery : rejectedDeliveries) {

        cout << "ID: " << delivery.id
             << " | Area: " << delivery.area
             << " | Weight: "
             << delivery.weight
             << " kg"
             << endl;
    }

}

// Main
int main() {
    const string filename = "data/input.csv";
    // 1.Read CSV
    vector<Delivery> deliveries =
        readDeliveriesFromCSV(filename);
    // 2.Handle empty input
    if (deliveries.empty()) {
        cout << "No deliveries found." << endl;
        return 0;
    }
    // 3.Validate deliveries
    vector<Delivery> rejectedDeliveries;
    vector<Delivery> validDeliveries =
        validateDeliveries(
            deliveries,
            rejectedDeliveries
        );
    // 4.Handle case where everything is invalid
    if (validDeliveries.empty()) {
        cout << "\nNo valid deliveries to schedule." << endl;
        printRejectedDeliveries(rejectedDeliveries);
        return 0;
    }
    // 5.Sort by priority
    sortDeliveries(validDeliveries);
    // 6.Create trips
    vector<Trip> trips =
        createTrips(validDeliveries);
    // 7.Print result
    printTrips(trips);
    // 8.Print rejected deliveries
    printRejectedDeliveries(rejectedDeliveries);

    // 9.Print summary
    printSummary(
        validDeliveries,
        rejectedDeliveries,
        trips
    );
    return 0;
}