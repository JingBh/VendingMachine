#ifndef VENDINGMACHINE_HASQUANTITY_H
#define VENDINGMACHINE_HASQUANTITY_H

class HasQuantity {
public:
    unsigned int getQuantity() const;

    void fill(unsigned int number);

    void draw(unsigned int number);

protected:
    HasQuantity()
        : quantity{0} {}

    explicit HasQuantity(unsigned int quantity)
        : quantity{quantity} {}

    unsigned int quantity;
};

#endif //VENDINGMACHINE_HASQUANTITY_H
