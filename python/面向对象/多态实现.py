class Animal(object):
    def eat(self):
        print('动物会吃')
class Dog(Animal):
    def eat(self):
        print('狗吃骨头...')
class Cat(Animal):
    def eat(self):
        print('猫吃鱼...')

def show(a):
    a.eat()

if __name__ == '__main__':
    show(Animal())
    show(Dog())
    show(Cat())
