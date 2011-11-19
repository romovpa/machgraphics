function features = RHOG(I, varargin)

%% settings
p = inputParser;
p.StructExpand = true;
p.addRequired('I');
p.addOptional('grX', [], @(x)(isnumeric(x) && isequal(size(x), size(I))));
p.addOptional('grY', [], @(x)(isnumeric(x) && isequal(size(x), size(I))));
p.addParamValue('cellSize', [6 6], @isnumeric);
p.addParamValue('blockSize', [3 3], @isnumeric);
p.addParamValue('bins', 9, @isscalar);
p.addParamValue('eps', 1e-3, @isscalar);
p.addParamValue('binsSigned', false, @islogical);
p.addParamValue('gradientScheme', [-1 0 1], @isnumeric);
p.addParamValue('verbose', false, @islogical);
p.parse(I, varargin{:});
param = p.Results;

%% algorithm


if size(I, 3) > 1
  I = rgb2gray(I);
end
I = im2double(I);

% gradient computation
grX = param.grX;
grY = param.grY;
if isempty(grX)
  grX = imfilter(I, param.gradientScheme);
end
if isempty(grY)
  grY = imfilter(I, param.gradientScheme');
end

% computation of hist in each cell
cellsNum = fix(size(I) ./ param.cellSize);
hists = zeros([param.bins cellsNum]);

% angles and magnitudes of pixels gradients
if param.binsSigned
  angles = (atan2(grY, grX) + pi) / (2*pi);
else
  angles = (atan(grY ./ grX) + pi/2) / pi;
end
magnitudes = sqrt(grX.^2 + grY.^2);

% computing the number of bin for each pixel
anglesBin = ones(size(I));
for b = 1:param.bins
  left = (b-1)/param.bins;
  right = b/param.bins;
  mask = (angles >= left) & (angles < right);
  anglesBin(mask) = b;
end

% computing histograms in cells
for r = 1:cellsNum(1)
  for c = 1:cellsNum(2)
    pixRows = (r-1)*param.cellSize(1)+1 : r*param.cellSize(1);
    pixCols = (c-1)*param.cellSize(2)+1 : c*param.cellSize(2);
    cellAngles = anglesBin(pixRows, pixCols);
    cellMagnitudes = magnitudes(pixRows, pixCols);
    for d = 1:param.bins
      hists(d,r,c) = sum(sum(cellMagnitudes( cellAngles == d )));
      %hists(d,r,c) = nnz( cellAngles == d );
    end
  end
end

% blocks normalization & feature vec construction
blocksNum = fix(cellsNum ./ param.blockSize);
blockLen = prod(param.blockSize)*param.bins;

F = zeros([blockLen, blocksNum]);
Z = [];
for r = 1:blocksNum(1)
  for c = 1:blocksNum(2)
    rows = (r-1)*param.blockSize(1)+1 : min(r*param.blockSize(1), cellsNum(1));
    cols = (c-1)*param.blockSize(2)+1 : min(c*param.blockSize(2), cellsNum(2));
    % feature vector
    f = hists(:,rows,cols);
    f = f(:);
    % normalization
    f = f / sqrt(f'*f + eps.^2);
    
    F(:,r,c) = f;
  end
end
features = F(:);

%% information
if param.verbose
  fprintf('Blocks: %dx%d\n', blocksNum);
end

end