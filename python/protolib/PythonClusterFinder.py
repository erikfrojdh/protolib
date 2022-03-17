import numpy as np
from skimage.measure import regionprops, label


def all_equal(mylist):
    """If all elements are equal return true otherwise false"""
    return all(x == mylist[0] for x in mylist)

class PythonClusterFinder:
    """
    Simple implementation of a Python based cluster finder
    using the tools from skimage. Used for benchmarking and validation
    of the C++ implementation. Has potentially more features out of 
    the box but is ~10x slower.
    """

    def __init__(self, threshold, check_equal = False):
        self.threshold = threshold
        self.check_equal = check_equal
        dt = [('row', np.int16), ('col', np.int16), ('energy', np.double), ('size', np.int16)]
        n_items_to_preallocate = 100
        self.clusters_ = np.zeros(n_items_to_preallocate, dtype = dt)
        self.n_clusters_found = 0


    def find_clusters(self, image):
        th_image = image > self.threshold
        labels = label(th_image, connectivity=2)
        for p in  regionprops(labels, intensity_image = image, cache = False):
            energy = p.intensity_image
            self.clusters_[self.n_clusters_found]['energy'] = energy.sum()
            self.clusters_[self.n_clusters_found]['size'] = p.area
            row,col = p.weighted_centroid
            self.clusters_[self.n_clusters_found]['row'] = row
            self.clusters_[self.n_clusters_found]['col'] = col
            self.n_clusters_found += 1
            if self.check_equal:
                arr = energy.flat[energy.flat>0]
                # return arr
                if not all_equal(arr):
                    return p
                # if not all_equal(energy.flat):
                #     pass
            if self.n_clusters_found == self.clusters_.size:
                before = self.clusters_.size
                self.clusters_.resize(self.clusters_.size*2)
                after = self.clusters_.size
                print(f"Resized cluster array from: {before} to {after}")
        return labels

    def hits(self):
        return self.clusters_[0:self.n_clusters_found]

    def total_clusters(self):
        return self.n_clusters_found