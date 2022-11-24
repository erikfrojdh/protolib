from ROOT import TH1D, TGraph, TCanvas
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

def plot(x, y, options='ALP', title="A TGraph",
         wx=900, wy=600, draw=True, error_graph=False,
         ex=None, ey=None):
    """Plot a TGraph

    Parameters
    ----------
    x: array_like
        Values for the x axis
    y: array_like
        Values for the y axis
    options: str, optional
        Draw options for the TGraph default is ALP -> (Axis, Line, Points)
    title: str, optional
        Title of the TGraph, defaults to A TGraph
    wx: int, optional
        Width of the plot in pixels
    wy: int, optional
        Height of the plot in pixels
    draw: bool, optional
        Defaults to True, draws the plot
    error_graph: bool, optional
        Use a TGraphErrors instead of a normal TGraph
    ex: array_like, optional
        Errors in x values
    ey: array_like, optional
        Errors in y values, if not spefified it is assumed to be np.sqrt(y)

    Returns
    ----------
    c: TCanvas
        Canvas on which the TGraph is drawn if draw is set to false
        c=None
    g: TGraph
        The graph.

    Raises
    ------
    ValueError
        If the size of x and y is not the same

    """
    #To have someting to return if draw is False
    c = None

    #Show stats box if the graph is fitted
    gStyle.SetOptFit(1)

    #type conversions (Note! as array does not copy unless needed)
    xdata = np.asarray(x, dtype=np.double)
    ydata = np.asarray(y, dtype=np.double)

    #check array size
    if xdata.size != ydata.size:
        raise ValueError("Size of x and y must match. (x.size: {:d}"\
                         ", y.size: {:d})".format(x.size, y.size))

    if draw:
        #Workaround assigning random name to the histogram to avoid
        #deleting it, TODO! do we have a smarter solution
        canvas_name = 'r.plotcanvas'+str(np.random.rand(1))
        canvas_title = 'plot'
        c = TCanvas(canvas_name, canvas_title, wx, wy)
        c.SetFillColor(0)
        c.SetGrid()

    if error_graph is False:
        g = TGraph(x.size, xdata, ydata)
    else:
        #Use a TErrorGraph to display error bars
        if ex is None:
            ex = np.zeros(x.size)
        if ey is None:
            ey = np.sqrt(y).astype(np.double)

        g = TGraphErrors(x.size, xdata, ydata, ex, ey)

    g.SetTitle(title)


    #Styling of the graph
    g.SetLineColor(style.line_color)
    g.SetLineWidth(style.line_width)
    g.SetMarkerColor(style.marker_color)
    g.SetMarkerStyle(style.marker_style)


    if draw:
        g.Draw(options)
        c.Update()

    return c, g