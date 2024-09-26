namespace cpp example

struct Person {
    1: i32 id,
    2: string name,
    3: i32 age
}

service PersonService {
    void ping(),
    Person getPerson(1: i32 id)
}
