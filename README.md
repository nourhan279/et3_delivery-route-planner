# Delivery Route Planner

A simple C++ program that organizes delivery requests into delivery trips while respecting a vehicle capacity of **10 kg per trip**.

The solution prioritizes urgent deliveries, tries to group deliveries from the same area, and makes sure that no trip exceeds the vehicle capacity.

---

## 1. Problem Overview

The delivery company receives a list of delivery requests.

Each delivery contains:

- **ID** – unique identifier of the delivery
- **Area** – delivery destination area
- **Priority** – lower number means higher urgency
- **Weight** – package weight in kilograms

The program organizes these deliveries into trips according to the following rules:

- A vehicle can carry a maximum of **10 kg per trip**.
- A trip must never exceed 10 kg.
- Deliveries from the same area should be grouped together when reasonably possible.
- Lower priority numbers are handled first.
- Every valid delivery must appear in exactly one trip.

---

## 2. Project Structure

```text
delivery-route-planner/
│
├── data/
│   └── deliveries.csv
│
├── src/
│   └── main.cpp
│
├── README.md
│
└── .gitignore
```

---

## 3. Input Format

I chose **CSV (Comma-Separated Values)** as the input format because it is simple, readable, and easy to edit or generate.

The input file is:

```text
data/deliveries.csv
```

### CSV Format

```csv
id,area,priority,weight
1,Nasr City,2,4.5
2,Maadi,1,2.0
3,Nasr City,3,1.2
4,Zamalek,1,7.0
5,Maadi,2,3.5
6,Nasr City,4,2.0
7,Maadi,3,1.5
```

### Columns

| Column     | Description                    |
| ---------- | ------------------------------ |
| `id`       | Delivery ID                    |
| `area`     | Delivery destination area      |
| `priority` | Lower number means more urgent |
| `weight`   | Package weight in kg           |

---

## 4. Requirements

The vehicle has a maximum capacity of:

```text
10 kg
```

The program makes sure that:

```text
Trip total weight <= 10 kg
```

A package that is heavier than 10 kg cannot be delivered because it cannot fit in a single trip.

---

## 5. Solution Approach

The algorithm works in several steps.

### Step 1: Read the CSV file

The program reads all delivery requests from:

```text
data/deliveries.csv
```

Each row is converted into a `Delivery` object containing:

```text
ID
Area
Priority
Weight
```

---

### Step 2: Validate the input

Before creating trips, the program checks each delivery.

A delivery is considered invalid if:

- Its weight is less than or equal to 0.
- Its weight is greater than 10 kg.
- Its area is empty.
- Its priority is less than or equal to 0.

Invalid deliveries are rejected and displayed separately.

Valid deliveries continue to the scheduling stage.

---

### Step 3: Sort deliveries by priority

Valid deliveries are sorted using:

```text
Priority → ID
```

Lower priority numbers are processed first because they represent more urgent deliveries.

If two deliveries have the same priority, their IDs are used as a deterministic tie-breaker.

For example:

```text
Priority 1, ID 2
Priority 1, ID 4
Priority 2, ID 1
Priority 2, ID 5
```

This keeps the processing order predictable.

---

### Step 4: Create trips

The program processes the sorted deliveries one by one.

For every delivery, it follows this decision process:

```text
New Delivery
     |
     v
Find a same-area trip that has enough capacity
     |
   Yes -----> Add delivery to that trip
     |
    No
     |
     v
Find any existing trip that has enough capacity
     |
   Yes -----> Add delivery to that trip
     |
    No
     |
     v
Create a new trip
```

### Same-area grouping

The program first tries to find an existing trip that:

1. Already contains a delivery from the same area.
2. Has enough remaining capacity.

This helps group deliveries going to the same area.

### Best-fit selection

If more than one suitable trip is available, the program chooses the trip that will have the **smallest remaining capacity after adding the delivery**.

For example:

```text
Trip 1 = 4 kg
Trip 2 = 6 kg
New delivery = 3 kg
```

Both trips can fit the delivery.

```text
Trip 1: 4 + 3 = 7 kg → 3 kg remaining
Trip 2: 6 + 3 = 9 kg → 1 kg remaining
```

Therefore, Trip 2 is selected because it leaves less unused capacity.

This is a simple **best-fit greedy approach**.

---

## 6. Example

Using the sample input:

```csv
id,area,priority,weight
1,Nasr City,2,4.5
2,Maadi,1,2.0
3,Nasr City,3,1.2
4,Zamalek,1,7.0
5,Maadi,2,3.5
6,Nasr City,4,2.0
7,Maadi,3,1.5
```

The processing order is:

```text
ID 2 → Priority 1
ID 4 → Priority 1
ID 1 → Priority 2
ID 5 → Priority 2
ID 3 → Priority 3
ID 7 → Priority 3
ID 6 → Priority 4
```

The resulting trips are:

```text
Trip 1:
ID 2 → Maadi → 2.0 kg
ID 4 → Zamalek → 7.0 kg

Total: 9.0 kg
```

```text
Trip 2:
ID 1 → Nasr City → 4.5 kg
ID 5 → Maadi → 3.5 kg
ID 3 → Nasr City → 1.2 kg

Total: 9.2 kg
```

```text
Trip 3:
ID 7 → Maadi → 1.5 kg
ID 6 → Nasr City → 2.0 kg

Total: 3.5 kg
```

Total deliveries:

```text
7
```

Total weight:

```text
21.7 kg
```

Number of trips:

```text
3
```

No trip exceeds the 10 kg capacity.

---

# 7. Edge Cases

The program handles the required edge cases as follows.

### 7.1 No deliveries

If the CSV contains only the header:

```csv
id,area,priority,weight
```

The program displays:

```text
No deliveries found.
```

No trips are created.

---

### 7.2 Package heavier than 10 kg

For example:

```csv
id,area,priority,weight
1,Maadi,1,12
2,Maadi,2,3
```

Delivery 1 is rejected because:

```text
12 kg > 10 kg
```

Delivery 2 is still processed normally.

This prevents an invalid package from breaking the rest of the delivery plan.

---

### 7.3 Package exactly 10 kg

A package weighing exactly 10 kg is valid.

For example:

```csv
id,area,priority,weight
1,Maadi,1,6
2,Maadi,2,4
```

Both deliveries can be placed in the same trip:

```text
6 + 4 = 10 kg
```

The trip reaches the maximum capacity but does not exceed it.

---

### 7.4 Adding the next package exceeds capacity

For example:

```csv
id,area,priority,weight
1,Maadi,1,7
2,Maadi,2,4
```

The two deliveries cannot be placed in the same trip because:

```text
7 + 4 = 11 kg
```

Therefore, a second trip is created.

---

### 7.5 Same priority

If several deliveries have the same priority, they are processed in ID order.

For example:

```csv
id,area,priority,weight
1,Maadi,1,2
2,Zamalek,1,3
3,Nasr City,1,1
```

The processing order is:

```text
ID 1
ID 2
ID 3
```

This provides a consistent tie-breaking rule.

---

# 8. Most Difficult Part

The most difficult part was deciding how to balance the two main requirements:

1. Handle urgent deliveries first.
2. Group deliveries from the same area when reasonably possible.

I decided to separate these responsibilities.

First, deliveries are sorted by priority so that urgent deliveries are always processed first.

Then, when assigning each delivery to a trip, the program first looks for a trip containing the same area.

If no suitable same-area trip exists, it considers any trip with enough remaining capacity.

This keeps the algorithm simple while still respecting both requirements.

---

# 9. Can the Algorithm Always Produce the Best Grouping?

No.

The algorithm is a **greedy algorithm**, so it makes the best decision available at the moment a delivery is processed. It does not go back and rearrange previous deliveries.

Because of this, there can be cases where another arrangement could use fewer trips or produce better area grouping.

For example, an early delivery may be placed into a trip because it fits, but later deliveries might have allowed a better combination if the earlier decision had been different.

The current solution prioritizes:

```text
Priority
   ↓
Same-area grouping
   ↓
Best use of remaining capacity
```

This makes the solution predictable and easy to understand, but it does not guarantee the mathematically optimal grouping for every possible input.

For this internship assignment, I chose this approach because the requirements focus on a reasonable and understandable solution rather than a guaranteed optimal bin-packing solution.

---

# 10. What Happens With 1,000,000 Deliveries?

With 1,000,000 delivery requests, memory usage would become an important consideration.

The program currently stores:

- All deliveries in memory.
- Rejected deliveries in memory.
- All created trips and their deliveries in memory.

The trip creation step can also become slower because, for each delivery, the program may scan many existing trips to find a suitable one.

In the worst case, this can approach:

```text
O(n × t)
```

where:

- `n` = number of deliveries
- `t` = number of created trips

Sorting the deliveries also requires:

```text
O(n log n)
```

For 1,000,000 deliveries, both memory usage and repeated trip searches could become significant.

A large input could therefore make the current implementation slower and more memory-intensive.

---

# 11. What Would I Improve With Another Day?

If I had another day, I would improve the solution in several areas.

### 1. More efficient trip lookup

Instead of checking every existing trip for every delivery, I could maintain additional data structures indexed by area.

This could reduce the amount of searching required when looking for same-area trips.

### 2. Better large-input handling

For very large input files, I could investigate processing the data in chunks instead of keeping everything in memory at once.

### 3. More test cases

I would add automated tests for:

- Empty input
- Invalid weights
- Weight exactly 10 kg
- Weight greater than 10 kg
- Same priority
- Same area
- Different areas
- Multiple trips
- Multiple trips with the same area
- Invalid CSV rows

### 4. Better CSV parsing

The current parser is intentionally simple. With more time, I would improve it to handle more CSV-specific cases such as fields containing commas or quotation marks.

### 5. Optional output file

I would add an option to save the generated delivery plan to a CSV or text output file instead of only printing it to the console.

---

# 12. Additional Feature

## Input Validation and Rejected Delivery Report and delivery summary

As an additional feature, I added input validation and a rejected-delivery report.

The original requirements do not explicitly require validation, but it is useful because real input data may contain invalid values.

The program checks for:

- Weight less than or equal to 0
- Weight greater than 10 kg
- Empty area
- Invalid priority

Invalid deliveries are not added to any trip.

Instead, the program reports them separately so that they do not cause the valid delivery plan to fail.

This feature was chosen because it makes the program safer and easier to understand when unexpected input is provided.

---

# 13. Output

The program displays:

- Each trip
- Deliveries inside each trip
- Delivery ID
- Area
- Priority
- Weight
- Total trip weight
- Remaining capacity
- Rejected deliveries
- Total number of valid deliveries
- Total number of rejected deliveries
- Total weight
- Number of trips
- Average trip weight

Example:

```text
================ DELIVERY PLAN ================

Trip 1
  Deliveries:
    ID: 2 | Area: Maadi | Priority: 1 | Weight: 2.0 kg
    ID: 4 | Area: Zamalek | Priority: 1 | Weight: 7.0 kg
  Total weight: 9.0 kg
  Remaining capacity: 1.0 kg

Trip 2
  Deliveries:
    ID: 1 | Area: Nasr City | Priority: 2 | Weight: 4.5 kg
    ID: 5 | Area: Maadi | Priority: 2 | Weight: 3.5 kg
    ID: 3 | Area: Nasr City | Priority: 3 | Weight: 1.2 kg
  Total weight: 9.2 kg
  Remaining capacity: 0.8 kg

Trip 3
  Deliveries:
    ID: 7 | Area: Maadi | Priority: 3 | Weight: 1.5 kg
    ID: 6 | Area: Nasr City | Priority: 4 | Weight: 2.0 kg
  Total weight: 3.5 kg
  Remaining capacity: 6.5 kg
```

---

# 14. How to Run

## Prerequisites

You need a C++ compiler such as **GCC/G++**.

The project was developed and tested using:

```text
G++ 16.1.0
```

The project uses only the C++ standard library, so no external libraries are required.

---

## Compile

From the project root directory:

```bash
g++ src/main.cpp -o delivery_planner
```

---

## Run

### On Linux/macOS/MSYS2

```bash
./delivery_planner
```

### On Windows

If using MSYS2 UCRT64:

```bash
./delivery_planner.exe
```

The program automatically reads:

```text
data/deliveries.csv
```

So make sure you run the program from the project root directory.

---

# 15. Testing

Before submission, I tested the program with the following cases:

### Test 1 – Empty input

```csv
id,area,priority,weight
```

Expected:

```text
No deliveries found.
```

### Test 2 – Package over capacity

```csv
id,area,priority,weight
1,Maadi,1,12
2,Maadi,2,3
```

Expected:

```text
Delivery 1 is rejected.
Delivery 2 is scheduled.
```

### Test 3 – Exactly 10 kg

```csv
id,area,priority,weight
1,Maadi,1,6
2,Maadi,2,4
```

Expected:

```text
One trip with total weight 10 kg.
```

### Test 4 – Capacity exceeded

```csv
id,area,priority,weight
1,Maadi,1,7
2,Maadi,2,4
```

Expected:

```text
Two trips.
```

### Test 5 – Same priority

```csv
id,area,priority,weight
1,Maadi,1,2
2,Zamalek,1,3
3,Nasr City,1,1
```

Expected:

```text
Deliveries are processed in ID order.
```

### Test 6 – Same-area grouping

```csv
id,area,priority,weight
1,Maadi,1,2
2,Nasr City,2,2
3,Maadi,3,3
4,Nasr City,4,2
```

Expected:

```text
The algorithm tries to keep deliveries from the same area together when capacity allows.
```

After testing, the original sample input should be restored in:

```text
data/deliveries.csv
```

---

# 16. Complexity

Let:

- `n` = number of deliveries
- `t` = number of trips

Sorting the deliveries takes:

```text
O(n log n)
```

For each delivery, the algorithm may search through the existing trips.

Therefore, trip creation can take approximately:

```text
O(n × t)
```

In the worst case, where the number of trips approaches the number of deliveries:

```text
O(n²)
```

The main memory usage is approximately:

```text
O(n + t)
```

because deliveries and created trips are stored in memory.

---

# 17. Design Decisions

The solution intentionally uses a simple structure instead of a complex architecture.

The main components are:

```text
Delivery
    ↓
CSV Reader
    ↓
Validation
    ↓
Priority Sorting
    ↓
Trip Creation
    ↓
Output & Summary
```

# 18. Technologies Used

- **C++**
- **GCC / G++**
- **CSV**
- **Standard C++ Library**

## Repository Structure

```text
delivery-route-planner/
├── data/
│   └── deliveries.csv
├── src/
│   └── main.cpp
├── README.md
└── .gitignore
```
