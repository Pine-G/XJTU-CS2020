import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import TextBox


pH = 0.5
pE = 0.3
LN = 0.2
LS = 1000.0


def EH(pE_S):
    pH_E = (LS * pH) / ((LS - 1) * pH + 1)
    pH_negE = (LN * pH) / ((LN - 1) * pH + 1)
    if 0 <= pE_S <= pE:
        pH_S = pH_negE + (pH - pH_negE) / pE * pE_S
    elif pE < pE_S <= 1:
        pH_S = pH + (pH_E - pH) / (1 - pE) * (pE_S - pE)
    return pH_S


func = np.vectorize(EH)
x = np.linspace(0, 1, 1000)
y = func(x)
fig, ax = plt.subplots()
ax.set_title('Bayes')
l, = plt.plot(x, y)

plt.xlim(0, 1)
plt.ylim(0, 1)
plt.xlabel('P(E|S)', x=1)
plt.ylabel('P(H|S)', y=1.04, rotation=0)
plt.subplots_adjust(bottom=.25)


def draw():
    y = func(x)
    l.set_ydata(y)
    plt.draw()


def submit1(expression):
    global pH
    pH = float(expression)
    draw()


pH_box = fig.add_axes([.15, .1, .3, .03])
pH_text_box = TextBox(pH_box, "p(H)", textalignment="center")
pH_text_box.set_val("0.5")
pH_text_box.on_submit(submit1)


def submit2(expression):
    global pE
    pE = float(expression)
    draw()


pE_box = fig.add_axes([.55, .1, .3, .03])
pE_text_box = TextBox(pE_box, "p(E)", textalignment="center")
pE_text_box.set_val("0.3")
pE_text_box.on_submit(submit2)


def submit3(expression):
    global LS
    LS = float(expression)
    draw()


LS_box = fig.add_axes([.15, .05, .3, .03])
LS_text_box = TextBox(LS_box, "LS", textalignment="center")
LS_text_box.set_val("1000.0")
LS_text_box.on_submit(submit3)


def submit4(expression):
    global LN
    LN = float(expression)
    draw()


LN_box = fig.add_axes([.55, .05, .3, .03])
LN_text_box = TextBox(LN_box, "LN", textalignment="center")
LN_text_box.set_val("0.2")
LN_text_box.on_submit(submit4)

plt.rc("font", family='YouYuan')
plt.text(0.45, 30.25, '计算机xxx-xxx-xxxxx', fontsize=10, color='black')
plt.show()
