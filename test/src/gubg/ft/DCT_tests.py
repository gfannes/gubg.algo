from gubg.ft import DCT
import numpy as np

def test_dct4():
    for n in (16,):
        for descr, create_origs_f in [
                ('eye', np.eye),
                # ('standard_normal', lambda n: np.random.standard_normal((n,n))),
                ]:
            # print( f'n:{n}, data:{descr}' )
            dct = DCT(n, t='sin')

            origs = create_origs_f(n)
            freqs = dct.fwd(origs)
            recon = dct.inv(freqs)
            if 1*'show':
                import matplotlib.pyplot as plt
                if 0*'imshow':
                    plt.imshow(freqs)
                    plt.colorbar()
                    plt.show()
                if 1*'plot':
                    for ix in range(0,n,4):
                        plt.plot(freqs[:,ix:ix+4])
                        plt.show()
            assert np.allclose(recon, origs)

