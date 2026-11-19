#include <fstream>
#include <iostream>

// Нарушен принцип разделения интерфейса: «толстый» интерфейс —
// три метода печати в одном интерфейсе. Клиенты вынуждены зависеть
// от методов, которые им не нужны.
class Printable
{
public:
    virtual ~Printable() = default;

    virtual std::string printAsHTML() const = 0;
    virtual std::string printAsText() const = 0;
    virtual std::string printAsJSON() const = 0;
};

// Нарушен принцип подстановки Барбары Лисков: Data наследует Printable,
// но не является полноценной заменой Printable: поведение методов
// зависит от format_ и может получить исключение.
class Data : public Printable
{
public:
    // Нарушен принцип открытости/закрытости: список форматов зашит
    // в enum; новый формат потребует правок здесь.
    enum class Format
    {
        kText,
        kHTML,
        kJSON
    };

    Data(std::string data, Format format)
        : data_(std::move(data)), format_(format) {}

    // Нарушен принцип подстановки Барбары Лисков: при подстановке Data
    // как Printable вызов printAsHTML может получить исключение,
    // если объект создан с другим форматом.
    std::string printAsHTML() const override
    {
        if (format_ != Format::kHTML) {
            throw std::runtime_error("Invalid format!");
        }
        return "<html>" + data_ + "<html/>";
    }

    // Нарушен принцип подстановки Барбары Лисков: аналогично —
    // метод действителен только для Format::kText.
    std::string printAsText() const override
    {
        if (format_ != Format::kText) {
            throw std::runtime_error("Invalid format!");
        }
        return data_;
    }

    // Нарушен принцип подстановки Барбары Лисков: аналогично —
    // метод действителен только для Format::kJSON.
    std::string printAsJSON() const override
    {
        if (format_ != Format::kJSON) {
            throw std::runtime_error("Invalid format!");
        }
        return "{ \"data\": \"" + data_ + "\"}";
    }

private:
    std::string data_;
    Format format_;
};

// Нарушен принцип открытости/закрытости: switch по Data::Format;
// при добавлении нового формата придётся изменять эту функцию.
void saveTo(std::ofstream& file, const Printable& printable, Data::Format format)
{
    switch (format)
    {
    case Data::Format::kText:
        file << printable.printAsText();
        break;
    case Data::Format::kJSON:
        file << printable.printAsJSON();
        break;
    case Data::Format::kHTML:
        file << printable.printAsHTML();
        break;
    }
}

// Нарушен принцип разделения интерфейса: обёртки заставляют клиента
// работать с Printable, в котором есть лишние для конкретного вызова методы.
void saveToAsHTML(std::ofstream& file, const Printable& printable) {
    saveTo(file, printable, Data::Format::kHTML);
}

// Нарушен принцип разделения интерфейса: аналогично — клиенту JSON
// не нужны HTML- и Text-методы Printable.
void saveToAsJSON(std::ofstream& file, const Printable& printable) {
    saveTo(file, printable, Data::Format::kJSON);
}

// Нарушен принцип разделения интерфейса: аналогично — клиенту Text
// не нужны HTML- и JSON-методы Printable.
void saveToAsText(std::ofstream& file, const Printable& printable) {
    saveTo(file, printable, Data::Format::kText);
}
