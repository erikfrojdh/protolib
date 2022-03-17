from ROOT import TH1D, TCanvas
import numpy as np
import matplotlib.pyplot as plt

def hist(data, xmin, xmax, bins=100, title = 'Histogram', print_info = True):
    canvas_name = f'canvas{np.random.randint(1000)}'
    canvas_title = 'Histogram'
    c = TCanvas(canvas_name, canvas_title, 200, 10, 900, 600)
    c.SetGrid()

    histogram_name = 'r.plothist'+str(np.random.randint(1000))
    h = TH1D(histogram_name, title, int(bins), xmin, xmax)

    if data is not None:
        hfill = h.Fill #slight speed up
        for x in np.nditer(data):
            hfill(x)
        del hfill

    h.Draw()
    c.Update()
    if print_info:
        print(f'{canvas_name=}')
        print(f'{histogram_name=}')

    return c,h

def get_hist(h):
    bins = h.GetNbinsX()
    x = np.zeros(bins)
    y = np.zeros(bins)
    for i in range(bins):
        x[i] = h.GetBinLowEdge(i+1)
        y[i] = h.GetBinContent(i+1)
    return x, y


def plot_hist(h):
    """Plot a ROOT histogram in a matplotlib figure"""
    x,y = get_hist(h)
    fig, ax = plt.subplots(figsize = (10,8))
    ax.step(x,y, where = 'post')
    ax.fill_between(x,y, step = 'post', alpha = 0.3)
    ax.set_ylim(0, y.max()*1.1)
    ax.set_xlim(x.min(), x.max())
    return fig, ax
