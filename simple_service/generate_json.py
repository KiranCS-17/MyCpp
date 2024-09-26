import json
import random
import string

# Helper function to generate random strings
def random_string(length):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

# Function to generate a large number of users with orders
def generate_large_json(num_users=100000, orders_per_user=10):
    users = []
    
    for i in range(num_users):
        user = {
            "id": i + 1,
            "name": random_string(10),
            "email": f"user{i+1}@example.com",
            "address": {
                "street": f"{random.randint(1, 9999)} {random_string(10)} St",
                "city": random_string(8),
                "zipcode": f"{random.randint(10000, 99999)}"
            },
            "phone": f"555-{random.randint(100, 999)}-{random.randint(1000, 9999)}",
            "orders": []
        }
        
        for j in range(orders_per_user):
            order = {
                "order_id": i * orders_per_user + j + 1,
                "product": random_string(15),
                "quantity": random.randint(1, 5),
                "price": round(random.uniform(10.0, 1000.0), 2)
            }
            user["orders"].append(order)
        
        users.append(user)
    
    return {"users": users}

# Generate the JSON
large_json = generate_large_json()

# Write to a file (this will create a large file around 200MB)
with open("large_data.json", "w") as f:
    json.dump(large_json, f)

