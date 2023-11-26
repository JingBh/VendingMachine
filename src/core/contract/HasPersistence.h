#ifndef VENDINGMACHINE_HASPERSISTENCE_H
#define VENDINGMACHINE_HASPERSISTENCE_H

#include <iosfwd>
#include <string>
#include <utility>

class HasPersistence {
public:
    HasPersistence() = delete;

    virtual void loadState();

    virtual void saveState() const;

protected:
    explicit HasPersistence(std::string name)
        : name{std::move(name)} {}

    ~HasPersistence() = default;

    virtual void saveState(std::ofstream &os) const = 0;

    virtual void loadState(std::ifstream &is) = 0;

private:
    const std::string name;

    std::string filePath;

    std::string getFilePath();

    std::string getFilePath() const;
};

#endif //VENDINGMACHINE_HASPERSISTENCE_H
